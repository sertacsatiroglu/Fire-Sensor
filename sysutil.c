#include "TM4C123GH6PM.h"
#include "sysutil.h"

void delay_us(uint32_t us) {

    int count = (int)((1000 * us) / (62.5));
    while (count--) {
        __asm("nop");
    }
}

void Delay(int ms)
{
    for (int i = 0; i < ms * 4000; i++)
    {
        __NOP();
    }
}