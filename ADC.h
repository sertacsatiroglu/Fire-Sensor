#ifndef ADC_H
#define ADC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "TM4C123GH6PM.h"


// ADC Channel Macros
#define AIN0   0
#define AIN1   1
#define AIN2   2
#define AIN3   3
#define AIN4   4
#define AIN5   5
#define AIN6   6
#define AIN7   7
#define AIN8   8
#define AIN9   9
#define AIN10  10
#define AIN11  11

// ADC Sequencer Macros
#define SEQ0   0
#define SEQ1   1
#define SEQ2   2
#define SEQ3   3

#ifdef __cplusplus
extern "C" {
#endif

// Function prototypes
void Init_ADC(uint8_t pin, uint8_t port);
uint16_t ReadAnalog(uint8_t sequencer);   
float ReadAnalog_offset(uint8_t sequencer);
float convert_volt(float readed);
void ADC_measurement(float *lm35_mV, float *threshold_mV);
void comparator_init(void);
#ifdef __cplusplus
}
#endif

// LM35 PE3, PC6
// TRESHOLD PE2, PC7

#endif // ADC_H
