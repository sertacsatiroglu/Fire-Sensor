#include "TM4C123GH6PM.h"
#include "I2C.h"


void i2c_init(void)
{
    //start clocks for i2c and gpio
    SYSCTL->RCGCGPIO |= 0x08;
    SYSCTL->RCGCI2C  |=  0x08;

    __asm("NOP");
    __asm("NOP");
    __asm("NOP");
    
    GPIOD->AFSEL |= 0x03;
		GPIOD->PCTL |= 0x33;
		GPIOD->ODR |= 0x02;
		GPIOD->DEN |= 0x03;
		

    //configure i2c
    I2C3->MCR= 0x10;

    /*Standart speed mode with speed of 100 kbps is more than fast enough 
    to read a single sensor data 128 times a second. From the data sheet 
    of the microcontroller, SCL period can be determined by this formula;
    SCL_PERIOD = 2 � (1 + TIMER_PRD) � (SCL_LP + SCL_HP) � CLK_PRD

    Since in standart mode, SCL clock speed is 100 kHz, we can exract 
    SCL clock period by this formula;
    TPR = (System Clock/(2*(SCL_LP + SCL_HP)*SCL_CLK))-1;
    TPR = (16MHz/(2*(6+4)*100000))-1;
    TPR = 7          */
    I2C3->MTPR = 0x07;
}

static int i2c_master_busy (void)
{
    while(I2C3->MCS & 0x01); // Check BUSY bit of MCS register
    return  I2C3->MCS & 0x0E; /*Check if data and adress is acknowledged by the sensor and if 
                                any errors occured. Ignore other bits and return 0 if no error. */
}

void i2c_bus_busy (void)
{
    while(I2C3->MCS & 0x40); // Check BUSBUSY bit of MCS register
    /*Unlike the master case where controller checks if address and data is acknowledged, there is
    no point in doing this here since ACK is itself generated by MCU*/
}

//Function to transmit 1 byte register address and then write 1 byte data to that register
int i2c_master_transmit(uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{   
    int error;
    I2C3->MSA = (slave_addr << 1) & 0xFE; //Transmit operation is specified by the LSB of the MSA register
    I2C3->MDR = reg_addr; //Register address to write
    I2C3->MCS = 0x03; //Start and run, master is in transmit state

    error = i2c_master_busy();
    if(error) return error;

    I2C3->MDR = data; 
    I2C3->MCS = 0x05; //Transmit followed by stop condition, master goes back to idle state

    error = i2c_master_busy();
    if(error) return error;

    return 0; //No error
} 

// Function to recieve data in anuy arbitrary length
int i2c_master_transmit_recieve(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t dataLenght)
{
    int error;
    I2C3->MSA = (slave_addr << 1) & 0xFE; //Transmit operation initially
    I2C3->MDR = reg_addr; //Address of the first register to be read
    I2C3->MCS = 0x03; //Start and run, same as in transmit

    error = i2c_master_busy();
    if(error) return error;

    I2C3->MSA = (slave_addr << 1) + 1; //Change to recieve operation
	
    /* Now master generates repeated start condition and after that 
    either reads a single register and generates NAK & stop condition or
    continues to read internal registers */
    if(dataLenght == 1) 
    {   
        I2C3->MCS = 0x07; //Start, run and stop condition
        error = i2c_master_busy();
        if(error) return error;
        *data++ = I2C3->MDR;
        i2c_bus_busy(); //Wait for the bus to be free
        return 0;
    }
	
    I2C3->MCS = 0x0B; //Start, run and ack condition
    error = i2c_master_busy();
    if(error) return error;
    *data++ = I2C3->MDR; //Read the data and increment the pointer to the next location
    
    I2C3->MCS = 0x05; //Run and stop condition
    error = i2c_master_busy();
    if(error) return error;
    *data++ = I2C3->MDR;
    i2c_bus_busy();
    return 0;
		
}
