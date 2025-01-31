#ifndef P_I2C_H
#define P_I2C_H

#include <stdint.h>

void i2c_init(void);
void i2c_write(uint8_t* data, uint8_t slave_addr, uint16_t len);
void i2c_read(uint8_t* data, uint8_t slave_addr, uint16_t len);

#endif
