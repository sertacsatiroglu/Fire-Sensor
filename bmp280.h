#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>
#include "bmp280_defs.h"
#include "I2C.h"

// Global variables
extern uint8_t id;
extern int32_t t_fine;
extern calibration_param_t calib_param;


void bmp280_init(void);
void bmp280_read_calibration_param(void);
void bmp280_BFI(uint8_t reg_addr, uint8_t data, uint8_t pos, uint8_t len);
int32_t bmp280_raw_temperature(void);
float bmp280_get_temp(void);

#endif // BMP280_H
