/*
 * ms5611.h
 * Driver for MS5611 pressure and temperature sensor (I2C, STM32 HAL)
 * Author: Jakub Zakrzewski
 * Date:   30.08.2025
 */


#ifndef PERIPHERALS_MS5611_H
#define PERIPHERALS_MS5611_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/** @brief 7-bit I2C address of MS5611 from datasheet */
#define MS5611_I2C_ADDR       0x77
/** @brief 8-bit I2C address for HAL functions */
#define MS5611_I2C_ADDR_HAL   (MS5611_I2C_ADDR << 1)

/* MS5611 commands */
#define MS5611_CMD_RESET        0x1E  /**< Reset command */
#define MS5611_CMD_CONV_D1      0x40  /**< Base command for pressure conversion */
#define MS5611_CMD_CONV_D2      0x50  /**< Base command for temperature conversion */
#define MS5611_CMD_ADC_READ     0x00  /**< Read ADC result */
#define MS5611_CMD_READ_PROM    0xA0  /**< Base command for PROM read */

/* Oversampling Ratio (OSR) settings */
#define MS5611_OSR_256          0x00  /**< OSR = 256  (0.60 ms conversion time) */
#define MS5611_OSR_512          0x02  /**< OSR = 512  (1.17 ms conversion time) */
#define MS5611_OSR_1024         0x04  /**< OSR = 1024 (2.28 ms conversion time) */
#define MS5611_OSR_2048         0x06  /**< OSR = 2048 (4.54 ms conversion time) */
#define MS5611_OSR_4096         0x08  /**< OSR = 4096 (9.04 ms conversion time) */

/**
 * @brief Initialize the MS5611 sensor and read calibration data.
 * @param hi2c Pointer to HAL I2C handle.
 * @param mathMode 0 = default constants, 1 = application note constants.
 */
void MS5611_Init(I2C_HandleTypeDef *hi2c, uint8_t mathMode);

/**
 * @brief Trigger a measurement and read temperature & pressure.
 * @param hi2c Pointer to HAL I2C handle.
 * @param temperature Pointer to store temperature in °C.
 * @param pressure Pointer to store pressure in Pa.
 */
void MS5611_Measure(I2C_HandleTypeDef *hi2c, float *temperature, float *pressure);

#ifdef __cplusplus
}
#endif

#endif /* PERIPHERALS_MS5611_H */
