/**
 * @file bmi088.c
 * @brief BMI088 6-axis IMU sensor driver implementation
 * @author Yusuf Karaböcek
 * @date July 2025
 */

#include "bmi088.h"
#include <stdint.h>

static int16_t combine_bytes(uint8_t msb, uint8_t lsb) {
    return (int16_t)(((uint16_t)msb << 8) | lsb);
}

int8_t bmi088_init(bmi088_t *dev) {
    if (!dev) return BMI088_ERR_NULL;

    uint8_t chip_id = 0;
    int8_t ret = 0;

    // ACC chip ID Kontrolü (Sadece İvmeölçer adresi)
    ret = dev->bus.read(BMI088_ACC_I2C_ADDR, BMI088_ACC_CHIP_ID, &chip_id, 1);
    if(ret != 0 || chip_id != BMI088_ACC_CHIP_ID_VAL) {
        return BMI088_ERR_INVALID;
    }

    // GYR chip ID Kontrolü (Sadece Jiroskop adresi)
    ret = dev->bus.read(BMI088_GYR_I2C_ADDR, BMI088_GYR_CHIP_ID, &chip_id, 1);
    if(ret != 0 || chip_id != BMI088_GYR_CHIP_ID_VAL) {
        return BMI088_ERR_INVALID;
    }

    // ACC Power up
    uint8_t data = BMI088_ACC_PWR_CTRL_ACC_ENABLE; // PWR_CTRL, normal mode
    ret = dev->bus.write(BMI088_ACC_I2C_ADDR, BMI088_ACC_PWR_CTRL, &data, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    // --- KRİTİK GECİKME: ACC'nin uyku modundan çıkıp uyanması için ---
    // Eğer MS5611 dosyasındaki safeDelay'i extern etmediysen,
    // FreeRTOS task'i içinde çalışacağın için buraya doğrudan osDelay(5); yazabilirsin.
    osDelay(5);

    // ACC Power Conf (sleep duration vb)
    data = 0x00;
    ret = dev->bus.write(BMI088_ACC_I2C_ADDR, BMI088_ACC_PWR_CONF, &data, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    // GYR Power up
    data = BMI088_GYR_PWR_CTRL_GYR_ENABLE; // normal mode
    ret = dev->bus.write(BMI088_GYR_I2C_ADDR, BMI088_GYR_LPM1, &data, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    // ACC ODR ve bandwidth ayarı - default 100Hz sample
    data = BMI088_ACC_ODR_100HZ; // 100Hz
    ret = dev->bus.write(BMI088_ACC_I2C_ADDR, BMI088_ACC_CONF, &data, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    // ACC Range ayarı - örnek +-6g
    data = BMI088_ACC_RANGE_6G;
    ret = dev->bus.write(BMI088_ACC_I2C_ADDR, BMI088_ACC_RANGE, &data, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    // GYR Range ayarı - örnek +- 2000 dps
    data = BMI088_GYR_RANGE_2000DPS;
    ret = dev->bus.write(BMI088_GYR_I2C_ADDR, BMI088_GYR_RANGE, &data, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    // GYR bandwidth ve ODR ayarı - örnek 100 Hz
    data = BMI088_GYR_ODR_100HZ;
    ret = dev->bus.write(BMI088_GYR_I2C_ADDR, BMI088_GYR_BW, &data, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    return 0;
}

int8_t bmi088_soft_reset(bmi088_t *dev) {
    if (!dev) return BMI088_ERR_NULL;

    uint8_t reset_cmd = 0xB6;
    int8_t ret;

    ret = dev->bus.write(dev->bus.dev_addr, BMI088_ACC_SOFTRESET, &reset_cmd, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    ret = dev->bus.write(dev->bus.dev_addr, BMI088_GYR_SOFTRESET, &reset_cmd, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    return BMI088_OK;
}

int8_t bmi088_read_accel(bmi088_t *dev, int16_t *accel_x, int16_t *accel_y, int16_t *accel_z) {
    if (!dev || !accel_x || !accel_y || !accel_z) return BMI088_ERR_NULL;

    uint8_t data[6];
    int8_t ret;

    ret = dev->bus.read(dev->bus.dev_addr, BMI088_ACC_X_LSB, data, 6);
    if(ret != 0) return BMI088_ERR_BUS;

    *accel_x = combine_bytes(data[1], data[0]);
    *accel_y = combine_bytes(data[3], data[2]);
    *accel_z = combine_bytes(data[5], data[4]);

    return BMI088_OK;
}

int8_t bmi088_read_gyro(bmi088_t *dev, int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z) {
    if (!dev || !gyro_x || !gyro_y || !gyro_z) return BMI088_ERR_NULL;

    uint8_t data[6];
    int8_t ret;

    ret = dev->bus.read(dev->bus.dev_addr, BMI088_GYR_X_LSB, data, 6);
    if(ret != 0) return BMI088_ERR_BUS;

    *gyro_x = combine_bytes(data[1], data[0]);
    *gyro_y = combine_bytes(data[3], data[2]);
    *gyro_z = combine_bytes(data[5], data[4]);

    return BMI088_OK;
}

int8_t bmi088_read_temp(bmi088_t *dev, int16_t *temp) {
    if (!dev || !temp) return BMI088_ERR_NULL;

    uint8_t data[2];
    int8_t ret;

    ret = dev->bus.read(dev->bus.dev_addr, BMI088_ACC_TEMP_MSB, data, 2);
    if(ret != 0) return BMI088_ERR_BUS;

    *temp = combine_bytes(data[0], data[1]);

    return BMI088_OK;
}

int8_t bmi088_read_all(bmi088_t *dev,
                    int16_t *accel_x, int16_t *accel_y, int16_t *accel_z,
                    int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z,
                    int16_t *temp) {
    if (!dev || !accel_x || !accel_y || !accel_z || !gyro_x || !gyro_y || !gyro_z || !temp)
        return BMI088_ERR_NULL;

    int8_t ret;

    ret = bmi088_read_accel(dev, accel_x, accel_y, accel_z);
    if(ret != BMI088_OK) return ret;

    ret = bmi088_read_gyro(dev, gyro_x, gyro_y, gyro_z);
    if(ret != BMI088_OK) return ret;

    ret = bmi088_read_temp(dev, temp);
    if(ret != BMI088_OK) return ret;

    return BMI088_OK;
}

int8_t bmi088_set_accel_odr(bmi088_t *dev, uint8_t odr) {
    if (!dev) return BMI088_ERR_NULL;

    uint8_t data;
    int8_t ret;

    ret = dev->bus.read(dev->bus.dev_addr, BMI088_ACC_CONF, &data, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    data = (data & 0x0F) | (odr << 4);

    ret = dev->bus.write(dev->bus.dev_addr, BMI088_ACC_CONF, &data, 1);
    return (ret == 0) ? BMI088_OK : BMI088_ERR_BUS;
}

int8_t bmi088_set_gyro_odr(bmi088_t *dev, uint8_t odr) {
    if (!dev) return BMI088_ERR_NULL;

    uint8_t data;
    int8_t ret;

    ret = dev->bus.read(dev->bus.dev_addr, BMI088_GYR_BW, &data, 1);
    if(ret != 0) return BMI088_ERR_BUS;

    data = (data & 0xF0) | (odr & 0x0F);

    ret = dev->bus.write(dev->bus.dev_addr, BMI088_GYR_BW, &data, 1);
    return (ret == 0) ? BMI088_OK : BMI088_ERR_BUS;
}

int8_t bmi088_set_accel_range(bmi088_t *dev, uint8_t range) {
    if (!dev) return BMI088_ERR_NULL;

    int8_t ret = dev->bus.write(dev->bus.dev_addr, BMI088_ACC_RANGE, &range, 1);
    return (ret == 0) ? BMI088_OK : BMI088_ERR_BUS;
}

int8_t bmi088_set_gyro_range(bmi088_t *dev, uint8_t range) {
    if (!dev) return BMI088_ERR_NULL;

    int8_t ret = dev->bus.write(dev->bus.dev_addr, BMI088_GYR_RANGE, &range, 1);
    return (ret == 0) ? BMI088_OK : BMI088_ERR_BUS;
}
