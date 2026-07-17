/*
 * ms5611.c
 * Driver for MS5611 pressure and temperature sensor (I2C, STM32 HAL)
 * Author: Jakub Zakrzewski
 * Date:   30.08.2025
 */


#include "ms5611.h"
#include <math.h>
#include "cmsis_os.h"

void safeDelay(uint32_t ms){
if(osKernelGetState() == osKernelRunning){
   	  osDelay(ms);
   	  }
   	  else{
   	   	  HAL_Delay(ms);
   	  }
}

/* Conversion delay for OSR=4096 (safe margin) */
#define MS5611_CONV_DELAY_MS   12

/* Calibration coefficients and constants */
static float coeff[7];

/* Send a single command byte to the sensor */
static void MS5611_SendCommand(I2C_HandleTypeDef *hi2c, uint8_t cmd)
{
    HAL_I2C_Master_Transmit(hi2c, MS5611_I2C_ADDR_HAL, &cmd, 1, 100);
}

/* Read 24-bit ADC result */
static uint32_t MS5611_ReadADC(I2C_HandleTypeDef *hi2c)
{
    uint8_t buf[3];
    HAL_I2C_Mem_Read(hi2c, MS5611_I2C_ADDR_HAL, MS5611_CMD_ADC_READ,
                     I2C_MEMADD_SIZE_8BIT, buf, 3, 100);
    return ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];
}

/* Read 16-bit PROM value at given index */
static uint16_t MS5611_ReadPROM(I2C_HandleTypeDef *hi2c, uint8_t index)
{
    uint8_t buf[2];
    HAL_I2C_Mem_Read(hi2c, MS5611_I2C_ADDR_HAL,
                     MS5611_CMD_READ_PROM + (index * 2),
                     I2C_MEMADD_SIZE_8BIT, buf, 2, 100);
    return (buf[0] << 8) | buf[1];
}

/* Initialize calculation constants */
static void MS5611_SetupConstants(uint8_t mathMode)
{
    coeff[0] = 1;
    coeff[1] = 32768L;          // SENSt1
    coeff[2] = 65536L;          // OFFt1
    coeff[3] = 3.90625E-3;      // TCS
    coeff[4] = 7.8125E-3;       // TCO
    coeff[5] = 256;             // Tref
    coeff[6] = 1.1920928955E-7; // TEMPSENS

    if (mathMode == 1) {        // Application note mode
        coeff[1] = 65536L;
        coeff[2] = 131072L;
        coeff[3] = 7.8125E-3;
        coeff[4] = 1.5625E-2;
    }
}

void MS5611_Init(I2C_HandleTypeDef *hi2c, uint8_t mathMode)
{
    /* Reset the sensor */
    MS5611_SendCommand(hi2c, MS5611_CMD_RESET);
    safeDelay(3);

    /* Load constants */
    MS5611_SetupConstants(mathMode);

    /* Read factory calibration coefficients */
    for (uint8_t reg = 0; reg < 7; reg++) {
        uint16_t promValue = MS5611_ReadPROM(hi2c, reg);
        coeff[reg] *= promValue;
    }
}

void MS5611_Measure(I2C_HandleTypeDef *hi2c, float *temperature, float *pressure)
{
    /* Start D1 (pressure) conversion */
    MS5611_SendCommand(hi2c, MS5611_CMD_CONV_D1 | MS5611_OSR_4096);
    safeDelay(MS5611_CONV_DELAY_MS);
    uint32_t D1 = MS5611_ReadADC(hi2c);

    /* Start D2 (temperature) conversion */
    MS5611_SendCommand(hi2c, MS5611_CMD_CONV_D2 | MS5611_OSR_4096);
    safeDelay(MS5611_CONV_DELAY_MS);
    uint32_t D2 = MS5611_ReadADC(hi2c);

    /* Calculate temperature and pressure */
    float dT   = D2 - coeff[5];
    float TEMP = 2000 + dT * coeff[6];

    float OFF  = coeff[2] + dT * coeff[4];
    float SENS = coeff[1] + dT * coeff[3];

    /* Second-order temperature compensation */
    if (TEMP < 2000) {
        float T2    = dT * dT * 4.6566128731E-10f;
        float tempDiff = (TEMP - 2000) * (TEMP - 2000);
        float OFF2  = 2.5f * tempDiff;
        float SENS2 = 1.25f * tempDiff;
        if (TEMP < -1500) {
            tempDiff = (TEMP + 1500) * (TEMP + 1500);
            OFF2  += 7 * tempDiff;
            SENS2 += 5.5f * tempDiff;
        }
        TEMP -= T2;
        OFF  -= OFF2;
        SENS -= SENS2;
    }

    float P = (D1 * SENS * 4.76837158205E-7f - OFF) * 3.051757813E-5f;

    if(temperature) *temperature = TEMP * 0.01f; /* °C */
    if(pressure)    *pressure    = (float)P;     /* Pa */
}
