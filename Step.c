#include "Step.h"


volatile uint8_t  startMotor = 0;    // When set to 1, motor will do its -90->+90->-90 sequence
volatile uint16_t stepno     = 0;    // Step index in [1..4], for a 4-step wave drive
              
int current_number = 0;							//  (We’ll cycle 1->2->3->4->1->... for CW, 4->3->2->1->4->... for CCW)
float MinMeas[];
float DistanceMeasurements[64];
volatile float closedistance = 0.0;
volatile int threshold = 30;
volatile float RealMin = 0;
volatile uint32_t RealAngle=0;
uint8_t j=0;
float distance = 0;

#define TOTAL_STEPS_180   1024        // Number of steps from -90° to +90° (assuming 1.8° per step)

#define STEP_DELAY_MS     10         // Delay between steps 

#define STEPPER_PINS_MASK 0xCC


// GPIO Configuration Structs
gpip_config pd2_config = {
    .pin_num = 2,
    .port = D,
    .PullUpR_Input = false,
    .PullDownR_Input = false,
    .InitialValue_Output = false, // Initial output value: 0
    .PinType = true,              // Pin type: Output
    .PinProperty = true,          // Pin property: Digital
    .afselTim = false             // Alternate function: GPIO (no timer)
};

gpip_config pd3_config = {
    .pin_num = 3,
    .port = D,
    .PullUpR_Input = false,
    .PullDownR_Input = false,
    .InitialValue_Output = false, // Initial output value: 0
    .PinType = true,              // Pin type: Output
    .PinProperty = true,          // Pin property: Digital
    .afselTim = false             // Alternate function: GPIO (no timer)
};

gpip_config pd6_config = {
    .pin_num = 6,
    .port = D,
    .PullUpR_Input = false,
    .PullDownR_Input = false,
    .InitialValue_Output = false, // Initial output value: 0
    .PinType = true,              // Pin type: Output
    .PinProperty = true,          // Pin property: Digital
    .afselTim = false             // Alternate function: GPIO (no timer)
};

gpip_config pd7_config = {
    .pin_num = 7,
    .port = D,
    .PullUpR_Input = false,
    .PullDownR_Input = false,
    .InitialValue_Output = false, // Initial output value: 0
    .PinType = true,              // Pin type: Output
    .PinProperty = true,          // Pin property: Digital
    .afselTim = false             // Alternate function: GPIO (no timer)
};



void init_step(void)
{
		
	  // Initialize GPIO pins
    HAL_GPIO_Init(&pd2_config);
    HAL_GPIO_Init(&pd3_config);
    HAL_GPIO_Init(&pd6_config);
    HAL_GPIO_Init(&pd7_config);
		HCSR04_Init();
}

void Stepper(uint16_t stepn)
{
    GPIOD->DATA &= ~STEPPER_PINS_MASK;
    
    switch(stepn)
    {
        case 1:
            // PD2 on -> 0x04
            GPIOD->DATA |= (0x04); 
            break;
        case 2:
            // PD3 on -> 0x08
            GPIOD->DATA |= (0x08); 
            break;
        case 3:
            // PD6 on -> 0x40
            GPIOD->DATA |= (0x40);
            break;
        case 4:
            // PD7 on -> 0x80
            GPIOD->DATA |= (0x80);
            break;
        default:
            // Should never happen, but just in case:
            break;
    }
}

/*****************************************************************************
 * STEP ONE STEP CLOCKWISE
 * In wave drive, we cycle stepno = 1->2->3->4->1->...
 ****************************************************************************/
void stepCW(void)
{
    stepno++;
    if (stepno > 4)
        stepno = 1;
    Stepper(stepno);
}

/*****************************************************************************
 * STEP ONE STEP COUNTERCLOCKWISE
 * In wave drive, we cycle stepno = 4->3->2->1->4->...
 ****************************************************************************/
void stepCCW(void)
{
		stepno--;
    if (stepno == 0)
        stepno = 4;
    Stepper(stepno);
}


/*****************************************************************************
 * RUN THE SEQUENCE:
 *  1) Start from -90 (logically).
 *  2) Rotate CW until +90 (100 steps).
 *  3) Rotate CCW back to -90 (100 steps).
 ****************************************************************************/
void runMotorSequence(void)
{
		while(1){
			if(current_number > 0){
			current_number--;
			if(current_number == 0){break;}
      stepCCW();
			Delay(2);}
			else{break;}
		}
    // From -90 -> +90 (CW):
    for(int i = 0; i < TOTAL_STEPS_180; i++)
    {	
			
				current_number++;
				if(i % 16 == 0){
						HCSR04_Measure(&distance);
						if((3 < distance) && (distance < 120)){DistanceMeasurements[j] = distance;}
						else{DistanceMeasurements[j] = 100;}
						j++;
						distance = 0.0;
						}
				if(GPIOC->MIS != 0x00){break;}
        stepCW();
				Delay(5);
    }
		if(GPIOC->MIS != 0x00){return;}
    // Now from +90 -> -90 (CCW):
    for(int i = 0; i < TOTAL_STEPS_180; i++)
    {
				if(GPIOC->MIS != 0x00){break;}
				current_number--;
				if(current_number == 0){break;}
        stepCCW();
        Delay(2);
    }
		GPIOD->DATA &= ~(0xCC);
}


void FindMinAndCompare() {
    float min = DistanceMeasurements[0];  
    int minIndex = 0;
    
    for (int i = 0; i < 64; i++) {  
        if (DistanceMeasurements[i] < min) {
						if(DistanceMeasurements[i] == 0){break;}
            min = DistanceMeasurements[i];
            minIndex = i;
        }
    }
		
    if((DistanceMeasurements[minIndex] < 100)){
				RealAngle= minIndex*2.81;
				RealMin = DistanceMeasurements[minIndex];
				LCDRadar();
				}
		else{
			Nokia5110_Clear();
			Nokia5110_SetCursor(0, 4);
			Nokia5110_OutString("NO OBJECT DETECTED!");
			return;}
		
}

void LCDRadar(){
			Nokia5110_Clear();
			Nokia5110_SetCursor(0, 4);
			Nokia5110_OutString("Dis:");
			
			Nokia5110_SetCursor(0, 5);
			PrintFloatToLCD(RealMin); 
			Nokia5110_OutString("CM");   
			Nokia5110_SetCursor(0, 1);
			Nokia5110_OutString("Angle:");
		
			Nokia5110_SetCursor(0, 2);
			PrintFloatToLCD(RealAngle); 
			Nokia5110_OutString("Deg");   
			
}

float RunMotor(){
			init_step();   
			runMotorSequence();
			if(current_number == 0){
			FindMinAndCompare();		
			}
			j=0;
			return RealMin;
}