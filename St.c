#include <stdio.h>
#include "TM4C123GH6PM.h"
#include "Serial.h"
#include "gpio.h"

volatile uint8_t  startMotor = 0;    // When set to 1, motor will do its -90->+90->-90 sequence
volatile uint16_t stepno     = 1;    // Step index in [1..4], for a 4-step wave drive
                                     //  (We’ll cycle 1->2->3->4->1->... for CW, 4->3->2->1->4->... for CCW)

#define TOTAL_STEPS_180   1024        // Number of steps from -90° to +90° (assuming 1.8° per step)
#define STEP_DELAY_MS     10         // Delay between steps (tweak as needed)
// Combined mask is 0x63 (binary 0110_0011).
#define STEPPER_PINS_MASK 0x63