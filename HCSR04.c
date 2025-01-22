#include "TM4C123GH6PM.h"
#include "sysutil.h"
#include "gpio.h"
#include "HCSR04.h"

#define SOUND_SPEED_CM_PER_US 0.0343

bool HCSR04_Init()
{
	gpip_config trigPin;
	trigPin.PinProperty = true;
	trigPin.PinType = true;
	trigPin.InitialValue_Output = false;
	trigPin.port = F;
	trigPin.pin_num = 4;
	trigPin.afselTim = false;
	
	HAL_GPIO_Init(&trigPin);
	
    SYSCTL->RCGCTIMER |= 1;     /* enable clock to Timer Block 0 */
    SYSCTL->RCGCGPIO |= 0x20;      /* enable clock to PORTB */
	
		GPIOF->LOCK = 0x4C4F434B;
		GPIOF->CR |= (1<<0);
    GPIOF->DIR &= ~(1<<0);        /* make PF0 an input pin */
    GPIOF->DEN |= (1<<0);         /* make PB6 as digital pin */
    GPIOF->AFSEL |= (1<<0);       /* use PB6 alternate function */
    GPIOF->PCTL &= ~0x0000000F;  /* configure PB6 for T0CCP0 */
    GPIOF->PCTL |= 0x00000007;
    
    TIMER0->CTL &= ~(1<<0);          /* disable timer0A during setup */
    TIMER0->CFG = 4;            /* Timer0A in 16-bit timer mode */
    TIMER0->TAMR = 0x17;        /* Timer0A enable up-count and capture mode */
    TIMER0->CTL |= (1<<3)|(1<<2);   /* capture rising and falling edges on PB6 pin */
	
	return true;
}

bool HCSR04_Measure(float *distance)
{
	TIMER0->CTL |= (1<<0);           /* enable Timer0A */
	(*((volatile unsigned long *)0xE000E410)) &= ~(0x200000000);
	(*((volatile unsigned long *)0xE000E410)) |= 0x200000000;
	
	HAL_GPIO_WritePin(F, 4, true);
	delay_us(10);             // Wait for 10 µs
	HAL_GPIO_WritePin(F, 4, false);

	unsigned int time_period;
  int risingEdge, fallingEdge;
	
   TIMER0->ICR = 4;            /* clear timer0A capture flag */
    while((TIMER0->RIS & (1<<2)) == 0) ;    /* wait till captured */
	  if(GPIOF->DATA & (1<<0)) /*check if rising edge occurs */
		{
    risingEdge = TIMER0->TAR;     /* save the timestamp */
		/* detect falling edge */
    TIMER0->ICR = 4;            /* clear timer0A capture flag */
    while((TIMER0->RIS & (1<<2)) == 0) ;    /* wait till captured */
    fallingEdge = TIMER0->TAR;     /* save the timestamp */
		}
		else
		{
			return false;
		}
	

	time_period = (fallingEdge - risingEdge) & 0x00FFFFFF;
	float width = (time_period)/16.0;
	*distance = (width * SOUND_SPEED_CM_PER_US) / 2.0;
	
	TIMER0->CTL &= ~(1<<0);           /* disable Timer0A */
	return true;
}