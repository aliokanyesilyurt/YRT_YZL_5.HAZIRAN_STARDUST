/**
 * @file bmi088.h
 * @brief BMI088 6-axis IMU sensor driver implementation
 * @author Yusuf Karaböcek
 * @date July 2025
 */

 #ifndef BMI088_H
 #define BMI088_H

 #include <stdint.h>
 #include <i2c_driver.h>

 // BMI088 register addresses

 #define BMI088_ACC_CHIP_ID 0x00 // Accelerometer chip ID
 #define BMI088_ACC_CHIP_ID_VAL 0x1E // Accelerometer chip ID value

 #define BMI088_ACC_CONF 0x40
 #define BMI088_ACC_RANGE 0x41

 #define BMI088_ACC_PWR_CONF 0x7C
 #define BMI088_ACC_PWR_CTRL 0x7D

 #define BMI088_ACC_SOFTRESET 0x7E

 #define BMI088_ACC_X_LSB 0x12
 #define BMI088_ACC_X_MSB 0x13
 #define BMI088_ACC_Y_LSB 0x14
 #define BMI088_ACC_Y_MSB 0x15
 #define BMI088_ACC_Z_LSB 0x16
 #define BMI088_ACC_Z_MSB 0x17

 #define BMI088_ACC_TEMP_MSB 0x22
 #define BMI088_ACC_TEMP_LSB 0x23

 #define BMI088_GYR_CHIP_ID 0x00 // Gyroscope chip ID
 #define BMI088_GYR_CHIP_ID_VAL 0x0F // Gyroscope chip ID value

 #define BMI088_GYR_RANGE 0x0F
 #define BMI088_GYR_BW 0x10

 #define BMI088_GYR_LPM1 0x11
 #define BMI088_GYR_SOFTRESET 0x14
 #define BMI088_GYR_CTRL 0x15

 #define BMI088_GYR_X_LSB 0x02
 #define BMI088_GYR_X_MSB 0x03
 #define BMI088_GYR_Y_LSB 0x04
 #define BMI088_GYR_Y_MSB 0x05
 #define BMI088_GYR_Z_LSB 0x06
 #define BMI088_GYR_Z_MSB 0x07

 // Essential registers for basic functionality
#define BMI088_ACC_STATUS 0x1D        // Accelerometer status register

// Accelerometer configuration registers
#define BMI088_ACC_ODR 0x40           // Output data rate
#define BMI088_ACC_OSR 0x40           // Oversampling rate
#define BMI088_ACC_BWP 0x40           // Bandwidth parameter

// Gyroscope configuration registers
#define BMI088_GYR_ODR 0x0F           // Output data rate
#define BMI088_GYR_FS 0x0F            // Full scale range
#define BMI088_GYR_BW 0x10            // Bandwidth

// Power management registers
#define BMI088_ACC_PWR_CTRL_ACC_ENABLE 0x04  // Enable accelerometer
#define BMI088_GYR_PWR_CTRL_GYR_ENABLE 0x01  // Enable gyroscope

#define BMI088_ACC_I2C_ADDR (0x18 << 1)  // Accelerometer I2C address
#define BMI088_GYR_I2C_ADDR (0x68 << 1)  // Gyroscope I2C address

// Configuration values
#define BMI088_ACC_RANGE_3G  0x00
#define BMI088_ACC_RANGE_6G  0x01
#define BMI088_ACC_RANGE_12G 0x02
#define BMI088_ACC_RANGE_24G 0x03

#define BMI088_GYR_RANGE_2000DPS  0x00
#define BMI088_GYR_RANGE_1000DPS  0x01
#define BMI088_GYR_RANGE_500DPS   0x02
#define BMI088_GYR_RANGE_250DPS   0x03
#define BMI088_GYR_RANGE_125DPS   0x04

// Data rate values
#define BMI088_ACC_ODR_12HZ  0x05
#define BMI088_ACC_ODR_25HZ  0x06
#define BMI088_ACC_ODR_50HZ  0x07
#define BMI088_ACC_ODR_100HZ 0x08
#define BMI088_ACC_ODR_200HZ 0x09
#define BMI088_ACC_ODR_400HZ 0x0A
#define BMI088_ACC_ODR_800HZ 0x0B
#define BMI088_ACC_ODR_1600HZ 0x0C

#define BMI088_GYR_ODR_2000HZ 0x00
#define BMI088_GYR_ODR_1000HZ 0x01
#define BMI088_GYR_ODR_400HZ  0x02
#define BMI088_GYR_ODR_200HZ  0x03
#define BMI088_GYR_ODR_100HZ  0x04
#define BMI088_GYR_ODR_50HZ   0x05
#define BMI088_GYR_ODR_25HZ   0x06
#define BMI088_GYR_ODR_12_5HZ 0x07

// Error codes
#define BMI088_OK           0
#define BMI088_ERR_NULL    -1
#define BMI088_ERR_BUS     -2
#define BMI088_ERR_INVALID -3
#define BMI088_ERR_TIMEOUT -4


 typedef enum {
    BMI088_BUS_I2C,
    BMI088_BUS_SPI
 } bmi088_bus_t;

 typedef struct {
    int8_t (*read)(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);
    int8_t (*write)(uint8_t dev_addr, uint8_t reg, const uint8_t *data, uint16_t len);
    uint8_t dev_addr;
 } bmi088_bus_interface_t;

typedef struct {
    bmi088_bus_t bus_type;
    bmi088_bus_interface_t bus;
} bmi088_t;

int8_t bmi088_init(bmi088_t *dev);
int8_t bmi088_soft_reset(bmi088_t *dev);
int8_t bmi088_read_accel(bmi088_t *dev, int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);
int8_t bmi088_read_gyro(bmi088_t *dev, int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);
int8_t bmi088_read_temp(bmi088_t *dev, int16_t *temp);
int8_t bmi088_read_all(bmi088_t *dev, int16_t *accel_x, int16_t *accel_y, int16_t *accel_z, int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z, int16_t *temp);
int8_t bmi088_set_accel_odr(bmi088_t *dev, uint8_t odr);
int8_t bmi088_set_gyro_odr(bmi088_t *dev, uint8_t odr);
int8_t bmi088_set_accel_range(bmi088_t *dev, uint8_t range);
int8_t bmi088_set_gyro_range(bmi088_t *dev, uint8_t range);

#endif /* BMI088_H */
