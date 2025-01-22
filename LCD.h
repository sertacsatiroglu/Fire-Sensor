// LCD.h
// Header file for Nokia 5110 LCD driver
#ifndef LCD_H
#define LCD_H

#include <stdint.h>

// Maximum dimensions of the screen
#define MAX_X 84
#define MAX_Y 48

void Nokia5110_Init(void);
void Nokia5110_Clear(void);
void Nokia5110_SetCursor(unsigned char newX, unsigned char newY);
void Nokia5110_OutChar(unsigned char data);
void Nokia5110_OutString(char *ptr);
void Nokia5110_OutUDec(unsigned short n);
void PrintFloatToLCD(float number);

typedef enum {
    SCREEN_GREET,
} ScreenState;

typedef union {

		struct {
		} greetData;

} ScreenData;

#endif