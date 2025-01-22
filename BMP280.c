#include "bmp280_defs.h"
#include "stdint.h"

void i2c_init(void);
int i2c_master_transmit(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
int i2c_master_transmit_recieve(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t dataLenght);
float bmp280_get_temp(void);
void bmp280_init(void);
void bmp280_read_calibration_param(void);
void bmp280_BFI(uint8_t reg_addr, uint8_t data, uint8_t pos, uint8_t len);
int32_t bmp280_raw_temperature(void);

uint8_t id;
static calibration_param_t calib_param;
void bmp280_init(void){
    char ret;
    ret = i2c_master_transmit_recieve(BMP280_SLAVE_ADDR, BMP280_ID_ADDR, &id, 1);
    if(ret != 0) while(1);
    if (id != BMP280_CHIP_ID) while (1);
    //reset the sensor
    uint8_t data = BMP280_RESET_VALUE;
    ret = i2c_master_transmit(BMP280_SLAVE_ADDR, BMP280_RESET_ADDR, data);
    if(ret != 0) while(1);
    for (int i = 0; i < 4000; i++) //Wait for a while to chip to be reset
    {
        __asm("NOP");
    }
    //read calibration parameters
    bmp280_read_calibration_param();

    
    bmp280_BFI(BMP280_CTRL_MEAS_ADDR, 0x00, 0, 2); //set the sensor to sleep mode before calibration
    bmp280_BFI(BMP280_CTRL_MEAS_ADDR, 0x00, 2, 3); //set pressure oversampling to 0
    bmp280_BFI(BMP280_CTRL_MEAS_ADDR, 0x01, 5, 3); //set temperature oversampling to 1 (LOW POWER MODE)
    bmp280_BFI(BMP280_CONFIG_ADDR, 0x00, 2, 3); //set filter to off
    bmp280_BFI(BMP280_CONFIG_ADDR, 0x00, 5, 3); //set standby time to 0.5 ms
    bmp280_BFI(BMP280_CTRL_MEAS_ADDR, 0x03, 0, 2); //set the sensor to normal mode
		
}

void bmp280_read_calibration_param(void){
    char ret;
    uint8_t udata[2];
    int8_t sdata[2];

    ret = i2c_master_transmit_recieve(BMP280_SLAVE_ADDR, BMP280_T1_ADDR, &udata, 2);
    if(ret != 0) while(1);
    calib_param.T1 = (((uint16_t)udata[1]) << 8) + (uint16_t)(udata[0]);
    ret = i2c_master_transmit_recieve(BMP280_SLAVE_ADDR, BMP280_T2_ADDR, &sdata, 2);
    if(ret != 0) while(1);
    calib_param.T2 = (((int16_t)sdata[1]) << 8) + (int16_t)(sdata[0]);
    ret = i2c_master_transmit_recieve(BMP280_SLAVE_ADDR, BMP280_T3_ADDR, &sdata, 2);
    if(ret != 0) while(1);
    calib_param.T3 = (((int16_t)sdata[1]) << 8) + (int16_t)(sdata[0]);
    return;
}

void bmp280_BFI(uint8_t reg_addr, uint8_t data, uint8_t pos, uint8_t len){
    char ret;
    uint8_t reg_data;
    i2c_master_transmit_recieve(BMP280_SLAVE_ADDR, reg_addr, &reg_data, 1);
    uint8_t mask = ((1 << len) - 1) << pos;
    reg_data &= ~mask;
    reg_data |= data << pos;
    ret = i2c_master_transmit(BMP280_SLAVE_ADDR, reg_addr, reg_data);
    if(ret != 0) while(1);
    return;
}
int32_t t_fine;
float bmp280_get_temp(void){
    int32_t adc_T = bmp280_raw_temperature();
    int32_t var1, var2, T;
    var1 = ((((adc_T>>3) - ((int32_t)calib_param.T1<<1))) * ((int32_t)calib_param.T2)) >> 11;
    var2 = (((((adc_T>>4) - ((int32_t)calib_param.T1)) * ((adc_T>>4) - ((int32_t)calib_param.T1))) >> 12) * ((int32_t)calib_param.T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
		
    return T;
}


int32_t bmp280_raw_temperature(void){
    char ret;
		uint8_t msb, lsb, xlsb;
    float temp;
    ret = i2c_master_transmit_recieve(BMP280_SLAVE_ADDR, BMP280_TEMP_ADDR, &msb, 1);
    if(ret != 0) while(1);
		ret = i2c_master_transmit_recieve(BMP280_SLAVE_ADDR, BMP280_TEMP_ADDR, &lsb, 1);
    if(ret != 0) while(1);
		ret = i2c_master_transmit_recieve(BMP280_SLAVE_ADDR, BMP280_TEMP_ADDR, &xlsb, 1);
    if(ret != 0) while(1);
    temp = ((int32_t)msb << 12) | ((int32_t)lsb << 4) | ((xlsb >> 4) & 0x0F);

    return temp;
}
