/* This example code can measure the pulse width or pulse duration of a wave signal */
/* Timer block 0 and sub timer A is configured in capture mode with both +ve and -ve edges detection */

/*header files for TM4C123 device and sprintf library */
#include "TM4C123GH6PM.h"
#include "sysutil.h"
#include "Serial.h"
#include "Step.h" 
#include "ADC.h"
#include "LCD.h"
#include "I2C.h"
#include "gpio.h"
#include "bmp280_defs.h"
#include "bmp280.h"
#include "gptimer.h"
#include <string.h>
#include <stdlib.h>


#define NVIC_EN0_R (*((volatile uint32_t *)0xE000E100))
#define NVIC_DIS0_R (*((volatile uint32_t *)0xE000E180))
#define NVIC_PRI0_R (*((volatile uint32_t *)0xE000E400))
#define NVIC_UNPEND0_R (*((volatile uint32_t *)0xE000E280))
#define SYS_CTRL_R 	(*((volatile unsigned long *)0xE000ED10))
	
// Global flags
volatile int adc_ready = 0; 
volatile int scan_req = 1;
volatile int scan_done = 0;
volatile int screen = 0;
int th_set = 0;
int c = 0;

// Global variables
uint32_t tempx = 20; 
uint32_t counterBuz = 350;
uint32_t pitch[6] = {32, 16, 8, 4, 2, 1}; // 0.5,1,2,4,8,16 MHz operations for buzzer
uint32_t chosenpitch = 0;

extern uint8_t keypadfunc(void);
extern void DELAY200(void);
extern void keypadgpio(void);
void initButton(void);
void system_init(void);
void state_machine(void);
ScreenData screenData;
void screen_function(ScreenState state, ScreenData *data);
void buzzer(void);

extern void i2c_init(void);
extern int i2c_master_transmit(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
extern int i2c_master_transmit_recieve(uint8_t slave_addr, uint8_t reg_addr, uint8_t *data, uint8_t dataLenght);
float bmp280_get_temp(void);
void bmp280_init(void);
void bmp280_read_calibration_param(void);
void bmp280_BFI(uint8_t reg_addr, uint8_t data, uint8_t pos, uint8_t len);
int32_t bmp280_raw_temperature(void);


static float temperatureA =0;
float temperatureC =0;
uint8_t state = 0;
uint8_t key[4] = {0,0,0,0};

float lm35_mV = 0;
float threshold_mV = 0;

gpip_config pe4_config = {
.pin_num = 4,
.port = E,
.PullUpR_Input = false,
.PullDownR_Input = false,
.InitialValue_Output = false, // Initial output value: 0
.PinType = true,              // Pin type: Output
.PinProperty = true,          // Pin property: Digital
.afselTim = false             // Alternate function: GPIO (no timer)
};

void choosepitch(void) {
    if (tempx < 32) {
        chosenpitch = pitch[0];
				counterBuz = counterBuz << 0;
    } else if (tempx >= 50 && tempx < 60) {
        chosenpitch = pitch[1];
				counterBuz = counterBuz << 1;
    } else if (tempx >= 38 && tempx < 50) {
        chosenpitch = pitch[2];
				counterBuz = counterBuz << 2;
    } else if (tempx >= 35 && tempx < 38) {
        chosenpitch = pitch[3];
				counterBuz = counterBuz << 3;
    } else if (tempx >= 32 && tempx < 35) {
        chosenpitch = pitch[4];
				counterBuz = counterBuz << 4;
    } else {
        chosenpitch = pitch[5];
				counterBuz = counterBuz << 5;
    }
}


void timer3_callback(uint32_t count_value, void *user_data) {
		TIMER3->CTL &= ~(0x01);
    if(counterBuz != 0){
			// Toggle the buzzer pin
			GPIOE->DATA ^= 0x10; //PF2
			TIMER3->ICR |=0x01; 
			TIMER3->CTL |= (0x01);
			counterBuz--;
		}
		if(counterBuz == 0){
			gptimer_stop(3);
			//HAL_GPIO_DisablePin(&pe4_config);
		}
		return;
}


void buzzer(void){
		HAL_GPIO_Init(&pe4_config);
		choosepitch(); //chosing frequency
		gptimer_config_t timer3_config = {
		.timer_num = 3,           
		.wide_timer = false,      
		.mode = GPTIMER_PERIODIC, 
		.count_up = false,        
		.prescaler = chosenpitch,           
		.load_value = (1600),  
		.count_mode = GPTIMER_BOTH_EDGE 
    };
		HAL_GPIO_Init(&pe4_config);
		gptimer_init(&timer3_config);
		gptimer_register_callback(3, &timer3_callback, 0);
		//gptimer_set_load(3,(32000/(chosenpitch)));
    gptimer_start(3);
}

gpip_config redLEDConfig = {
        .pin_num = 1,           // PF1: Red LED
        .port = F,              // Port F
        .PullUpR_Input = false, // No pull-up resistor (not needed for output)
        .PullDownR_Input = false, // No pull-down resistor
        .InitialValue_Output = false, // Initially off
        .PinType = true,        // Output
        .PinProperty = true,    // Digital pin
        .afselTim = false       // No alternate function
    };

    gpip_config blueLEDConfig = {
        .pin_num = 2,           // PF2: Blue LED
        .port = F,              // Port F
        .PullUpR_Input = false,
        .PullDownR_Input = false,
        .InitialValue_Output = false,
        .PinType = true,
        .PinProperty = true,
        .afselTim = false
    };

    gpip_config greenLEDConfig = {
        .pin_num = 3,           // PF3: Green LED
        .port = F,              // Port F
        .PullUpR_Input = false,
        .PullDownR_Input = false,
        .InitialValue_Output = false,
        .PinType = true,
        .PinProperty = true,
        .afselTim = false
    };

void initLEDs(void) {
    // Initialize LEDs
    HAL_GPIO_Init(&redLEDConfig);
    HAL_GPIO_Init(&blueLEDConfig);
    HAL_GPIO_Init(&greenLEDConfig);
}

void controlLEDsBasedOnDistance(float DISTANCE) {
		initLEDs();
    if (DISTANCE >= 75.0f && DISTANCE <= 100.0f) {
        // Turn on Green LED (PF3), Turn off Red (PF1) and Blue (PF2)
        HAL_GPIO_WritePin(F, 3, true);
        HAL_GPIO_WritePin(F, 1, false);
        HAL_GPIO_WritePin(F, 2, false);
    } else if (DISTANCE >= 50.0f && DISTANCE < 75.0f) {
        // Turn on Blue LED (PF2), Turn off Red (PF1) and Green (PF3)
        HAL_GPIO_WritePin(F, 2, true);
        HAL_GPIO_WritePin(F, 1, false);
        HAL_GPIO_WritePin(F, 3, false);
    } else if (DISTANCE < 50.0f && DISTANCE > 0) {
        // Turn on Red LED (PF1), Turn off Green (PF3) and Blue (PF2)
        HAL_GPIO_WritePin(F, 1, true);
        HAL_GPIO_WritePin(F, 3, false);
        HAL_GPIO_WritePin(F, 2, false);
    } else {
        // Turn off all LEDs if distance is out of range
        HAL_GPIO_WritePin(F, 1, false);
        HAL_GPIO_WritePin(F, 2, false);
        HAL_GPIO_WritePin(F, 3, false);
		}
}


// Analog comparator interrupt handler
void COMP0_Handler(void) {
    if (COMP->ACMIS & 0x01) {     
        COMP->ACMIS |= 0x01;     
        NVIC->ICPR[0] = (1 << 25); 

        
        COMP->ACINTEN &= ~0x01;   
        NVIC->ICER[0] = (1 << 25); 

        COMP->ACCTL0 &= ~0x01;    

        adc_ready = 1;            
        state = 1;
        scan_req = 1;
    }
}

void GPIOB_Handler(void){
		Nokia5110_Clear();
    Nokia5110_SetCursor(0, 1);
    Nokia5110_OutString("Temperature");
    Nokia5110_SetCursor(0, 2);
    Nokia5110_OutString("Limit: ");
		char buffer[16] = "";       
    int buffer_index = 0;       
    int key = 0;                
		while(1){
		key = keypadfunc(); 
        
        if (key >= 0 && key <= 9) {
            if (buffer_index < 15) {
                buffer[buffer_index++] = '0' + key; 
                buffer[buffer_index] = '\0';        
            }
        } else if (key == 11) {
            if (buffer_index < 15 && strchr(buffer, '.') == NULL) {
                buffer[buffer_index++] = '.';       
                buffer[buffer_index] = '\0';        
            }
        } else if (key == 15) {
            if (buffer_index > 0) {
                temperatureA = atof(buffer); 
            } else {
                temperatureA = 1000.0; 
            }
            break; 
        } else if (key == 10) {
            if (buffer_index > 0) {
                buffer[--buffer_index] = '\0';   
            }
        }

        // Updating the display with the current buffer
        Nokia5110_SetCursor(7, 2);                
        Nokia5110_OutString("       ");           
        Nokia5110_SetCursor(7, 2);
        Nokia5110_OutString(buffer);              
				Delay(100);
    }
		if(th_set == 0){
		Nokia5110_Clear();
    Nokia5110_SetCursor(0, 2);
    Nokia5110_OutString("Set to: ");
    PrintFloatToLCD(temperatureA); 
		}
		th_set = 0;
		GPIOB->ICR |= 0xFF;
		NVIC_DIS0_R |= 0x02;
		NVIC_EN0_R |= 0x04;
}

void GPIOC_Handler(void){
	Delay(200);
	GPIOC->MIS &= 0xF0;
	if(GPIOC->MIS == 0x10){
		state=0;
	}
	else if(GPIOC->MIS == 0x20){
		if(screen == 0){
					screen = 1;
					}
		else if(screen == 1){
					screen = 0;
		}
		else if(screen == 2){
					screen = 0;
		}
	}
	else if(GPIOC->MIS == 0x30){
		GPIOC->ICR |= 0x30;
		NVIC_EN0_R |= 0x02; 
		th_set = 1;
	}
	GPIOC->ICR |= 0x30;
	
}


void state_machine(void){
		while(1){
			if(state==0){//Deep sleep state
				NVIC_DIS0_R |= 0x3; //Disable button interrupts
				counterBuz = 350;
				HAL_GPIO_DisablePin(&redLEDConfig);
				HAL_GPIO_DisablePin(&blueLEDConfig);
				HAL_GPIO_DisablePin(&greenLEDConfig);
				GPIOE->DATA &= ~(0x20);// turn off power led 
				char buffer[9];
				Nokia5110_Clear();
				Nokia5110_SetCursor(0, 2);
				Nokia5110_OutString("Treshold:");
				Nokia5110_SetCursor(5, 3);
				sprintf(buffer, "%.2f", temperatureA);  // Format the number
				Nokia5110_OutString(buffer);
				adc_ready = 0;
				scan_req =1;
				state =1;
				screen = 1;
				 COMP->ACMIS |= 0x01;       // Clear any pending comparator interrupt
				 COMP->ACINTEN |= 0x01;     // Enable comparator interrupt
				 NVIC->ISER[0] = (1 << 25); // Enable comparator interrupt in NVIC
				 __ASM("WFI");              // Wait for interrupt (enter deep sleep)
			}
			else if(state==1){ //waiting for digital temp reading reach treshold
				c++;
				NVIC_DIS0_R |= 1 << 25; //disable comparator interrupt, only use it to wake from deep sleep
				GPIOE->DATA |= 0x20;// turn on power led 
				GPIOB->ICR |= 0xFF;
				NVIC_EN0_R |= 0x4; //enable interrupt for port C,
				while(1){ //exit this loop only when treshold exceeds limit
					if (adc_ready) {
							ADC_measurement(&lm35_mV, &threshold_mV);  // Take the ADC measurement
					}
					if(th_set == 0){
					temperatureC = bmp280_get_temp(); 
					}
					if(temperatureC > (temperatureA * 100) && (scan_done == 0 || scan_req == 1)){
						state = 2;
						scan_req = 0;
						screen = 2;
					}
					if(state != 1) break;
					if(screen == 0){
					char buffer[20];
								Nokia5110_Clear();
                Nokia5110_SetCursor(0, 1);
                Nokia5110_OutString("Analog Temp");
                Nokia5110_SetCursor(0, 2);
                Nokia5110_OutString("LM35:");
                Nokia5110_SetCursor(6, 2);
								float temp_t = lm35_mV / 10.00;
                sprintf(buffer, "%.2f", temp_t);
                Nokia5110_OutString(buffer);

                Nokia5110_SetCursor(0, 3);
                Nokia5110_OutString("Analog TH:");
                Nokia5110_SetCursor(6, 3);
                float temp_t2 = threshold_mV / 10.00;
                sprintf(buffer, "%.2f", temp_t2);
                Nokia5110_OutString(buffer);
						Delay(100);
					}
					if(screen == 1){
					char buffer[20];
								Nokia5110_Clear();
                Nokia5110_SetCursor(0, 1);
								Nokia5110_OutString("Digital Temp:");
                Nokia5110_SetCursor(5, 2);
								float temp_t = temperatureC / 100.00;
                sprintf(buffer, "%.2f", temp_t);
                Nokia5110_OutString(buffer);
                Nokia5110_SetCursor(0, 3);
                Nokia5110_OutString("Digital TH:");
                Nokia5110_SetCursor(5, 4);
                sprintf(buffer, "%.2f C", temperatureA);
                Nokia5110_OutString(buffer);
						Delay(100);
					}
				}
			}
			else if(state ==2){
				NVIC_DIS0_R |= 0x2; 
				buzzer();
				int distance = RunMotor();
				if(state == 2){
				initLEDs();
				controlLEDsBasedOnDistance(distance);
				state = 1;
				scan_done = 1;
				screen = 2;
				}
			}
		}
}

int main(void)
{		
		system_init();
		state_machine();
		while(1);
}

void system_init(void){
		
		initButton();
		i2c_init();
    __asm("NOP");
    __asm("NOP");
    __asm("NOP");
    bmp280_init();
		
		SYS_CTRL_R |= 0x04; //enable deep sleep 
		Nokia5110_Init();
		screen_function(SCREEN_GREET, &screenData);
		Init_ADC(3,4); // Port E (4), Pin 3 (AIN0)
    Init_ADC(2,4); // Port E (4), Pin 2 (AIN1)
		keypadgpio();
		NVIC_EN0_R |= 0x2; //enable interrupt for port B
		__asm("WFI");
		comparator_init();
		COMP->ACMIS = 0x01;
		NVIC_DIS0_R |= 0x2; //disable interrupt for port B
		GPIOE->DATA &= ~(0x20);// turn off power led 
		NVIC_EN0_R |= (1 << 25); //enable comparator interrupt
		__asm("WFI"); //wait in deep sleep until comparator interrupts
}




