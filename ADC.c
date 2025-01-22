#include "ADC.h"

#define DC_OFFSET	(0xFFF/3.33)


void Init_ADC(uint8_t pin, uint8_t port){
			if(port == 1){
						if(pin == 4){
								SYSCTL->RCGCGPIO |= (1 << 1);  // Enable clock for Port B
								SYSCTL->RCGCADC |= (1 << 0);   // Enable clock for ADC0
								while ((SYSCTL->PRGPIO & (1 << 1)) == 0); // Wait until Port B is ready

								// Configure PB4 as an analog input
								GPIOB->AFSEL |= (1 << 4);
								GPIOB->DEN &= ~(1 << 4);
								GPIOB->AMSEL |= (1 << 4);

								// Configure ADC0
								ADC0->ACTSS &= ~(1 << 3);
								ADC0->EMUX &= ~0xF000;
								ADC0->SSMUX3 = 10;
								ADC0->SSCTL3 = 0x06;
								ADC0->ACTSS |= (1 << 3);
						}
						else if(pin == 5){
								SYSCTL->RCGCGPIO |= (1 << 1);
								SYSCTL->RCGCADC |= (1 << 0);
								while ((SYSCTL->PRGPIO & (1 << 1)) == 0);

								GPIOB->AFSEL |= (1 << 5);
								GPIOB->DEN &= ~(1 << 5);
								GPIOB->AMSEL |= (1 << 5);

								ADC0->ACTSS &= ~(1 << 3);
								ADC0->EMUX &= ~0xF000;
								ADC0->SSMUX3 = 11;
								ADC0->SSCTL3 = 0x06;
								ADC0->ACTSS |= (1 << 3);
						}
			}
			else if(port == 3){
								if(pin == 0){
									SYSCTL->RCGCGPIO |= (1 << 3);
									SYSCTL->RCGCADC |= (1 << 0);
									while ((SYSCTL->PRGPIO & (1 << 3)) == 0);

									GPIOD->AFSEL |= (1 << 0);
									GPIOD->DEN &= ~(1 << 0);
									GPIOD->AMSEL |= (1 << 0);

									ADC0->ACTSS &= ~(1 << 3);
									ADC0->EMUX &= ~0xF000;
									ADC0->SSMUX3 = 7;
									ADC0->SSCTL3 = 0x06;
									ADC0->ACTSS |= (1 << 3);
								}
								else if(pin == 1){
									SYSCTL->RCGCGPIO |= (1 << 3);
									SYSCTL->RCGCADC |= (1 << 0);
									while ((SYSCTL->PRGPIO & (1 << 3)) == 0);

									GPIOD->AFSEL |= (1 << 1);
									GPIOD->DEN &= ~(1 << 1);
									GPIOD->AMSEL |= (1 << 1);

									ADC0->ACTSS &= ~(1 << 3);
									ADC0->EMUX &= ~0xF000;
									ADC0->SSMUX3 = 6;
									ADC0->SSCTL3 = 0x06;
									ADC0->ACTSS |= (1 << 3);
								}
								else if(pin == 2){
									SYSCTL->RCGCGPIO |= (1 << 3);
									SYSCTL->RCGCADC |= (1 << 0);
									while ((SYSCTL->PRGPIO & (1 << 3)) == 0);

									GPIOD->AFSEL |= (1 << 2);
									GPIOD->DEN &= ~(1 << 2);
									GPIOD->AMSEL |= (1 << 2);

									ADC0->ACTSS &= ~(1 << 3);
									ADC0->EMUX &= ~0xF000;
									ADC0->SSMUX3 = 5;
									ADC0->SSCTL3 = 0x06;
									ADC0->ACTSS |= (1 << 3);
								}
								else if(pin == 3){
									SYSCTL->RCGCGPIO |= (1 << 3);
									SYSCTL->RCGCADC |= (1 << 0);
									while ((SYSCTL->PRGPIO & (1 << 3)) == 0);

									GPIOD->AFSEL |= (1 << 3);
									GPIOD->DEN &= ~(1 << 3);
									GPIOD->AMSEL |= (1 << 3);

									ADC0->ACTSS &= ~(1 << 3);
									ADC0->EMUX &= ~0xF000;
									ADC0->SSMUX3 = 4;
									ADC0->SSCTL3 = 0x06;
									ADC0->ACTSS |= (1 << 3);
								}
			}else if(port == 4){
								if(pin == 0){
										SYSCTL->RCGCGPIO |= (1 << 4);  // Enable clock for Port E
										SYSCTL->RCGCADC |= (1 << 0);   // Enable clock for ADC0
										while ((SYSCTL->PRGPIO & (1 << 4)) == 0);
										GPIOE->AFSEL |= (1 << 0);
										GPIOE->DEN &= ~(1 << 0);
										GPIOE->AMSEL |= (1 << 0);

										ADC0->ACTSS &= ~(1 << 3);
										ADC0->EMUX &= ~0xF000;
										ADC0->SSMUX3 = 3;
										ADC0->SSCTL3 = 0x06;
										ADC0->ACTSS |= (1 << 3);
								}
								else if(pin == 1){
										SYSCTL->RCGCGPIO |= (1 << 4);  // Enable clock for Port E
										SYSCTL->RCGCADC |= (1 << 0);   // Enable clock for ADC0
										while ((SYSCTL->PRGPIO & (1 << 4)) == 0);
										GPIOE->AFSEL |= (1 << 1);
										GPIOE->DEN &= ~(1 << 1);
										GPIOE->AMSEL |= (1 << 1);
										ADC0->ACTSS &= ~(1 << 3);
										ADC0->EMUX &= ~0xF000;
										ADC0->SSMUX3 = 2;
										ADC0->SSCTL3 = 0x06;
										ADC0->ACTSS |= (1 << 3);
								}
								else if(pin == 2){
										SYSCTL->RCGCGPIO |= (1 << 4);  // Enable clock for Port E
										SYSCTL->RCGCADC |= (1 << 1);   // Enable clock for ADC1
										while ((SYSCTL->PRGPIO & (1 << 4)) == 0);
										GPIOE->AFSEL |= (1 << 2);
										GPIOE->DEN &= ~(1 << 2);
										GPIOE->AMSEL |= (1 << 2);
										ADC1->ACTSS &= ~(1 << 3);
										ADC1->EMUX &= ~0xF000;
										ADC1->SSMUX3 = 1;
										ADC1->SSCTL3 = 0x06;
										ADC1->ACTSS |= (1 << 3);
								}
								else if(pin == 3){
										SYSCTL->RCGCGPIO |= (1 << 4);  // Enable clock for Port E
										SYSCTL->RCGCADC |= (1 << 0);   // Enable clock for ADC0
										while ((SYSCTL->PRGPIO & (1 << 4)) == 0);
										GPIOE->AFSEL |= (1 << 3);
										GPIOE->DEN &= ~(1 << 3);
										GPIOE->AMSEL |= (1 << 3);

										ADC0->ACTSS &= ~(1 << 3);
										ADC0->EMUX &= ~0xF000;
										ADC0->SSMUX3 = 0;
										ADC0->SSCTL3 = 0x06;
										ADC0->ACTSS |= (1 << 3);
								}
								else if(pin == 4){
										SYSCTL->RCGCGPIO |= (1 << 4);  // Enable clock for Port E
										SYSCTL->RCGCADC |= (1 << 0);   // Enable clock for ADC0
										while ((SYSCTL->PRGPIO & (1 << 4)) == 0);
										GPIOE->AFSEL |= (1 << 3);
										GPIOE->DEN &= ~(1 << 3);
										GPIOE->AMSEL |= (1 << 3);

										ADC0->ACTSS &= ~(1 << 3);
										ADC0->EMUX &= ~0xF000;
										ADC0->SSMUX3 = 0;
										ADC0->SSCTL3 = 0x06;
										ADC0->ACTSS |= (1 << 3);
								}
								else if(pin == 5){
									SYSCTL->RCGCGPIO |= (1 << 4);  // Enable clock for Port E
									SYSCTL->RCGCADC |= (1 << 0);   // Enable clock for ADC0
									while ((SYSCTL->PRGPIO & (1 << 4)) == 0);
									GPIOE->AFSEL |= (1 << 5);
									GPIOE->DEN &= ~(1 << 5);
									GPIOE->AMSEL |= (1 << 5);

									ADC0->ACTSS &= ~(1 << 3);
									ADC0->EMUX &= ~0xF000;
									ADC0->SSMUX3 = 8;
									ADC0->SSCTL3 = 0x06;
									ADC0->ACTSS |= (1 << 3);
								}
			}
			return;
}

uint16_t ReadAnalog(uint8_t sequencer) {
			ADC0->PSSI = (1 << sequencer);              // Start ADC conversion
			int read_value = 0;

			while ((ADC0->RIS & (1 << sequencer)) == 0); // Wait for conversion to complete
			switch (sequencer) {
					case 0:
							read_value = ADC0->SSFIFO0 & 0xFFF;
							break;
					case 1:
							read_value = ADC0->SSFIFO1 & 0xFFF;
							break;
					case 2:
							read_value = ADC0->SSFIFO2 & 0xFFF;
							break;
					case 3:
							read_value = ADC0->SSFIFO3 & 0xFFF;
							break;
					default:
							read_value = 0; // Handle invalid sequencer error (optional)
							break;
			}
    ADC0->ISC = (1 << sequencer); 
    return read_value;
}

void ADC_measurement(float *lm35_mV, float *threshold_mV){
    int16_t result;
    float lm35_ema = 0.0; // Exponential Moving Average for LM35
    float threshold_ema = 0.0; // Exponential Moving Average for Threshold
    const float alpha = 0.1f;  // Smoothing factor for EMA
    const uint32_t offset_correction = 5; // Offset correction in mV

    // Measure LM35 (ADC0, PE3) 1000 times
    for (int i = 0; i < 1000; i++) {
        ADC0->PSSI |= 0x08;              // Start conversion
        while ((ADC0->RIS & 0x08) == 0); // Wait for conversion to complete
        result = ADC0->SSFIFO3 & 0x0FFF; // 12-bit result
        ADC0->ISC |= 0x08;               // Clear interrupt flag
        float mV = (result * 3300.0f) / 4095.0f; // Convert ADC value to millivolts (3300mV for 3.3V ref)
        lm35_ema = (alpha * mV) + ((1 - alpha) * lm35_ema); // Apply EMA
    }
    *lm35_mV = (float)(lm35_ema + offset_correction); // Add offset correction and output mV

    // Measure Threshold (ADC1, PE2) 1000 times
    for (int i = 0; i < 1000; i++) {
        ADC1->PSSI |= 0x08;              // Start conversion
        while ((ADC1->RIS & 0x08) == 0); // Wait for conversion to complete
        result = ADC1->SSFIFO3 & 0x0FFF; // 12-bit result
        ADC1->ISC |= 0x08;               // Clear interrupt flag
        float mV = (result * 3300.0f) / 4095.0f; // Convert ADC value to millivolts
        threshold_ema = (alpha * mV) + ((1 - alpha) * threshold_ema); // Apply EMA
    }
    *threshold_mV = (float)(threshold_ema + offset_correction); // Add offset correction and output mV
}

// Analog comparator initialization
void comparator_init(void) {
    SYSCTL->RCGCACMP |= 0x01; // Enable comparator clock
    SYSCTL->RCGCGPIO |= 0x04; // Enable clock for GPIO Port C

    GPIOC->AFSEL |= 0xC0; // Enable alternate function for PC4 (AIN0) and PC5 (AIN1)
    GPIOC->DEN &= ~0xC0;  // Disable digital functionality
    GPIOC->AMSEL |= 0xC0; // Enable analog functionality

    COMP->ACREFCTL = 0;         // Disable internal reference, use external (trimpot)
		COMP->ACCTL0 &= 0xF101;			
    COMP->ACCTL0 |= 0x02C8;  // VIN+ = AIN0, VIN- = AIN1, interrupt on rising edge

    COMP->ACMIS |= 0x1;          
    COMP->ACINTEN |= 0x01;     

		//	NVIC REGISTERS
		(*((volatile unsigned long *)0xE000E418)) &= 0xFFFF00FF;
		(*((volatile unsigned long *)0xE000E418)) |= 0x00006000; //set priority as 3

}


float ReadAnalog_offset(uint8_t sequencer) {
			ADC0->PSSI = (1 << sequencer);	// Start ADC conversion
			float read_value_off = 0;
			read_value_off = ReadAnalog(sequencer) - (1.65*DC_OFFSET); //1.65 OFFSET!!!
			return read_value_off;
}


float convert_volt(float readed){

		return readed*3.3/4095;
}
