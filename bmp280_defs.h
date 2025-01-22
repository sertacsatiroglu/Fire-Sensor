#include "stdint.h"
#ifndef __BMP280_DEFS_H__
#define __BMP280_DEFS_H__
// Define static device parameters as to be used in memonic form as much as possible

#define BMP280_CHIP_ID 0x58
#define BMP280_RESET_VALUE 0xB6
#define BMP280_SLAVE_ADDR 0x76

// Define internal register addresses of BMP280 sensor

#define BMP280_ID_ADDR 0xD0
#define BMP280_RESET_ADDR 0xE0
#define BMP280_STATUS_ADDR 0xF3
#define BMP280_CTRL_MEAS_ADDR 0xF4
#define BMP280_CONFIG_ADDR 0xF5
#define BMP280_TEMP_ADDR 0xFA  //  0xFA - 0xFC (bit 7,6,5,4)

#define BMP280_T1_ADDR 0x88 // 2 bytes 
#define BMP280_T2_ADDR 0x8A // 2 bytes
#define BMP280_T3_ADDR 0x8C // 2 bytes

// Define the calibration parameters for the BMP280 sensor

typedef struct 
{
  uint16_t T1; 
  int16_t T2;  
  int16_t T3;  
} calibration_param_t;

#endif
