
/*
#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include "esp_err.h"
#include "esp_driver_i2c.h"

// Default I2C pins
#define I2C_MASTER_SCL_IO           22      // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO           21      // GPIO number for I2C master data
#define I2C_MASTER_FREQ_HZ          400000  // I2C master clock frequency (400kHz)
#define I2C_MASTER_PORT             I2C_NUM_0

// Initialize I2C master
esp_err_t i2c_master_init(void);

// Write bytes to I2C device
esp_err_t i2c_master_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);

// Read bytes from I2C device
esp_err_t i2c_master_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, size_t len);

#endif // I2C_MASTER_H 
*/

