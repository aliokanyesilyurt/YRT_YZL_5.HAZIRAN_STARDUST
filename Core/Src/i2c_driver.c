/**
 * @file i2c_driver.c
 * @brief I2C communication driver implementation for BMI088
 * @author Yusuf Karaböcek
 * @date July 2025
 */

#include "i2c_driver.h"
#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c2;

static i2c_driver_t g_i2c_driver = {0};

static int stm32_i2c1_read_callback(uint8_t device_addr, uint8_t reg, uint8_t *data, uint16_t len) {
    return HAL_I2C_Mem_Read(&hi2c2, device_addr, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
}

static int stm32_i2c1_write_callback(uint8_t device_addr, uint8_t reg, uint8_t *data, uint16_t len) {
    return HAL_I2C_Mem_Write(&hi2c2, device_addr, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
}

static int stm32_i2c2_read_callback(uint8_t device_addr, uint8_t reg, uint8_t *data, uint16_t len) {
    return HAL_I2C_Mem_Read(&hi2c2, device_addr, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
}

static int stm32_i2c2_write_callback(uint8_t device_addr, uint8_t reg, uint8_t *data, uint16_t len) {
    return HAL_I2C_Mem_Write(&hi2c2, device_addr, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
}

int i2c_driver_init(i2c_driver_t *driver, i2c_read_callback_t read_cb, i2c_write_callback_t write_cb) {
    if (!driver || !read_cb || !write_cb) {
        return -1;
    }

    driver->read_callback = read_cb;
    driver->write_callback = write_cb;

    g_i2c_driver = *driver;
    return 0;
}

int i2c_driver_init_i2c1(void) {
    return i2c_driver_init(&g_i2c_driver, stm32_i2c1_read_callback, stm32_i2c1_write_callback);
}

int i2c_driver_init_i2c2(void) {
    return i2c_driver_init(&g_i2c_driver, stm32_i2c2_read_callback, stm32_i2c2_write_callback);
}

int8_t i2c_read_wrapper(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len) {
    if (g_i2c_driver.read_callback) {
        return (int8_t)g_i2c_driver.read_callback(dev_addr, reg, data, len);
    }
    return (HAL_I2C_Mem_Read(&hi2c2, dev_addr, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK) ? 0 : -1;
}

int8_t i2c_write_wrapper(uint8_t dev_addr, uint8_t reg, const uint8_t *data, uint16_t len) {
    if (g_i2c_driver.write_callback) {
        return (int8_t)g_i2c_driver.write_callback(dev_addr, reg, (uint8_t*)data, len);
    }
    return (int8_t)stm32_i2c1_write_callback(dev_addr, reg, (uint8_t*)data, len);
}
