#ifndef DRIVER_HCSR04_H
#define DRIVER_HCSR04_H

#include <stdbool.h>
#include <stdint.h>
#include "gpio.h"

bool HCSR04_Init();

bool HCSR04_Measure(float* distance);

#endif // DRIVER_HCSR04_H