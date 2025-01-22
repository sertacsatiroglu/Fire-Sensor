#ifndef I2C_H
#define I2C_H


#include <stdint.h>

void i2c_init(void);
int i2c_master_transmit(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
int i2c_master_transmit_recieve(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t dataLenght);

#endif