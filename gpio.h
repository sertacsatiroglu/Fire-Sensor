#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>
#include <stdint.h>



//*****************************************************************************
// The gpio_port enum
//*****************************************************************************
typedef enum {
    A = 0, // Port A
    B = 1, // Port B
    C = 2, // Port C
    D = 3, // Port D
    E = 4, // Port E
    F = 5, // Port F
} gpio_port;


//*****************************************************************************
// The gpip_config struct
//   pin_num:            The pin number (0..7)
//   port:               Port (A..F)
//   PullUpR_Input:      true to enable pull-up resistor (if input)
//   PullDownR_Input:    true to enable pull-down resistor (if input)
//   InitialValue_Output:true for high-level output at init
//   PinType:            true for Output, false for Input
//   PinProperty:        true for Digital, false for Analog
//*****************************************************************************
typedef struct {
    uint8_t     pin_num;
    gpio_port   port;
    bool        PullUpR_Input;
    bool        PullDownR_Input;
    bool        InitialValue_Output;
    bool        PinType;       // 1 - Output ; 0 - Input
    bool        PinProperty;   // 1 - Digital ; 0 - Analog
		bool				afselTim; //timer for pin input
} gpip_config;


typedef void (*gpio_callback_t)(uint32_t count_value, void *user_data);
//*****************************************************************************
// Public function prototypes
//*****************************************************************************
void HAL_GPIO_Init(const gpip_config *config);
void HAL_GPIO_WritePin(gpio_port port, uint8_t pin, bool value);
bool HAL_GPIO_ReadPin(gpio_port port, uint8_t pin);
void HAL_GPIO_DisablePin(const gpip_config *config);
int HAL_GPIO_RegisterCallback(gpio_port port, uint8_t pin, gpio_callback_t callback, void *user_data);
void gpio_start_interrupt(gpio_port port);
void gpio_disable_interrupt(gpio_port port);
void initButton(void);


#endif // HAL_GPIO_H