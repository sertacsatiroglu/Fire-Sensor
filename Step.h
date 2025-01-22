#ifndef DRIVER_STEP_H
#define DRIVER_STEP_H

#include "TM4C123GH6PM.h"
#include "sysutil.h"
#include "gpio.h"
#include "HCSR04.h"
#include "LCD.h"
#include "Serial.h"



void init_step(void);

void Stepper(uint16_t stepn);

void stepCW(void);

void stepCCW(void);

void runMotorSequence(void);

void FindMinAndCompare(void);

void LCDRadar(void);

float RunMotor(void);

	
#endif // DRIVER_STEP_H