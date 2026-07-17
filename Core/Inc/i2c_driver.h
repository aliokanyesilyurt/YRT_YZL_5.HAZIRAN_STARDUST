/**
 * @file i2c_driver.h
 * @brief I2C communication driver for BMI088
 * @author Yusuf Karaböcek
 * @date July 2025
 */

#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>

/**
 * @brief I2C read callback function type
 */
typedef int (*i2c_read_callback_t)(uint8_t device_addr, uint8_t reg, uint8_t *data, uint16_t len);

/**
 * @brief I2C write callback function type
 */
typedef int (*i2c_write_callback_t)(uint8_t device_addr, uint8_t reg, uint8_t *data, uint16_t len);

/**
 * @brief I2C driver structure
 */
typedef struct {
    i2c_read_callback_t read_callback;   /**< Read callback function */
    i2c_write_callback_t write_callback; /**< Write callback function */
} i2c_driver_t;

/**
 * @brief Default wrapper functions for STM32 HAL
 */
int8_t i2c_read_wrapper(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);
int8_t i2c_write_wrapper(uint8_t dev_addr, uint8_t reg, const uint8_t *data, uint16_t len);

/**
 * @brief Initialize I2C driver
 * @param driver Pointer to driver structure
 * @param device_addr Device I2C address
 * @param read_cb Read callback function
 * @param write_cb Write callback function
 * @return 0 on success, negative value on error
 */
int i2c_driver_init(i2c_driver_t *driver, i2c_read_callback_t read_cb, i2c_write_callback_t write_cb);

/**
 * @brief Initialize I2C1 driver with default callbacks
 * @param device_addr Device I2C address
 * @return 0 on success, negative value on error
 */
int i2c_driver_init_i2c1(void);

/**
 * @brief Initialize I2C2 driver with default callbacks
 * @param device_addr Device I2C address
 * @return 0 on success, negative value on error
 */
int i2c_driver_init_i2c2(void);

#endif // I2C_DRIVER_H
