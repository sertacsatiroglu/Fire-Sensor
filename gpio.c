//*****************************************************************************
// Includes or definitions for register-level programming on TM4C123GH6PM
// (Addresses are from the TM4C123GH6PM datasheet).
//*****************************************************************************
#include "TM4C123GH6PM.h"
#include "gpio.h"
// Base addresses for GPIO ports
#define GPIO_PORTA_BASE  0x40004000
#define GPIO_PORTB_BASE  0x40005000
#define GPIO_PORTC_BASE  0x40006000
#define GPIO_PORTD_BASE  0x40007000
#define GPIO_PORTE_BASE  0x40024000
#define GPIO_PORTF_BASE  0x40025000

// Offsets for GPIO registers (for each port)
#define GPIO_DATA_OFFSET     0x3FC  // Data register (maskable)
#define GPIO_DIR_OFFSET      0x400  // Direction register
#define GPIO_AFSEL_OFFSET    0x420  // Mode control: 0 = GPIO
#define GPIO_PUR_OFFSET      0x510  // Pull-up resistor
#define GPIO_PDR_OFFSET      0x514  // Pull-down resistor
#define GPIO_DEN_OFFSET      0x51C  // Digital enable
#define GPIO_AMSEL_OFFSET    0x528  // Analog mode select
#define GPIO_LOCK_OFFSET     0x520  // GPIO Lock
#define GPIO_CR_OFFSET       0x524  // GPIO Commit

// Clock gating control for GPIO
#define SYSCTL_RCGCGPIO_R    (*((volatile uint32_t *)0x400FE608))

// Magic values
#define GPIO_LOCK_KEY        0x4C4F434B  // Unlock the GPIOCR register


//*****************************************************************************
// Helper inline: return base address from gpio_port enum
//*****************************************************************************
static inline uint32_t GPIO_PortBase(gpio_port port)
{
    switch (port)
    {
        case A: return GPIO_PORTA_BASE;
        case B: return GPIO_PORTB_BASE;
        case C: return GPIO_PORTC_BASE;
        case D: return GPIO_PORTD_BASE;
        case E: return GPIO_PORTE_BASE;
        case F: return GPIO_PORTF_BASE;
        default: return 0;
    }
}

//*****************************************************************************
// Enable clock for a given port
//*****************************************************************************
static inline void GPIO_EnableClock(gpio_port port)
{
    SYSCTL_RCGCGPIO_R |= (1 << port);
			__ASM("NOP");
			__ASM("NOP");
			__ASM("NOP");
}


// Internal structure to hold registered callbacks
typedef struct {
    gpio_callback_t callback;
    void *user_data;
} gpio_callback_info_t;

gpio_callback_info_t gpio_callbacks[6][8];

//*****************************************************************************
// HAL_GPIO_Init
//   1. Enable clock to GPIO
//   2. Unlock port (if needed) for PF0 or PD7, etc.
//   3. Set direction (input/output)
//   4. Configure pull-up/down
//   5. Enable digital or analog
//   6. Set initial output value if output
//*****************************************************************************
void HAL_GPIO_Init(const gpip_config *config)
{
    // 1. Enable clock
    GPIO_EnableClock(config->port);

    // 2. Get base address
    uint32_t base = GPIO_PortBase(config->port);

		//Check lock pins.
		if ((config->port == F) && (config->pin_num == 0)) {
        (*((volatile uint32_t *)(base + GPIO_LOCK_OFFSET))) = GPIO_LOCK_KEY;
         (*((volatile uint32_t *)(base + GPIO_CR_OFFSET)))   |= (1 << 0);
		}
		
		if ((config->port == D) && (config->pin_num == 7)) {
        (*((volatile uint32_t *)(base + GPIO_LOCK_OFFSET))) = GPIO_LOCK_KEY;
         (*((volatile uint32_t *)(base + GPIO_CR_OFFSET)))   |= (1 << 7);
		}

    // 3. Direction register (GPIODIR)
    volatile uint32_t *dirReg = (volatile uint32_t *)(base + GPIO_DIR_OFFSET);
    if (config->PinType) {*dirReg |= (1 << config->pin_num);} 
		else {*dirReg &= ~(1 << config->pin_num);}

    // 4. Pull-up / Pull-down registers
    volatile uint32_t *purReg = (volatile uint32_t *)(base + GPIO_PUR_OFFSET);
    volatile uint32_t *pdrReg = (volatile uint32_t *)(base + GPIO_PDR_OFFSET);

    // Clear existing pull configuration for that pin
    *purReg &= ~(1 << config->pin_num);
    *pdrReg &= ~(1 << config->pin_num);

    if (!config->PinType) // If input
    {
        if (config->PullUpR_Input) {
            *purReg |= (1 << config->pin_num);
        }
        else if (config->PullDownR_Input) {
            *pdrReg |= (1 << config->pin_num);
        }
    }

    // 5. Digital Enable or Analog
    volatile uint32_t *denReg   = (volatile uint32_t *)(base + GPIO_DEN_OFFSET);
    volatile uint32_t *amselReg = (volatile uint32_t *)(base + GPIO_AMSEL_OFFSET);
    volatile uint32_t *afselReg = (volatile uint32_t *)(base + GPIO_AFSEL_OFFSET);

    // Clear bits in AFSEL and AMSEL by default for a “regular” GPIO
    *afselReg &= ~(1 << config->pin_num);
    *amselReg &= ~(1 << config->pin_num);
    *denReg   &= ~(1 << config->pin_num);
		
		//timer input
		if(config->afselTim == true){
			GPIOB->AFSEL		|= 0x4;
			GPIOB->PCTL			&= ~(0x00000F00);
			GPIOB->PCTL			|= (0x00000700); //PB4 Timer 1 
		}
		
		
    if (config->PinProperty) {
        // Digital
        *denReg |= (1 << config->pin_num);
    } else {
        // Analog
        // (Depending on your application, you may need to set AFSEL for ADC input, etc.)
        *amselReg |= (1 << config->pin_num);
    }

    // 6. If output, set initial value
    if ((config->PinType)&&(config->InitialValue_Output == 1)) {
        HAL_GPIO_WritePin(config->port, config->pin_num, 1);
    }
		else if((config->PinType)&&(config->InitialValue_Output == 0)){
        HAL_GPIO_WritePin(config->port, config->pin_num, 0);
    }
}

//*****************************************************************************
// HAL_GPIO_WritePin
//   - Write 1 or 0 to the given pin
//*****************************************************************************
void HAL_GPIO_WritePin(gpio_port port, uint8_t pin, bool value)
{
    uint32_t base = GPIO_PortBase(port);
    volatile uint32_t *dataReg = (volatile uint32_t *)(base + GPIO_DATA_OFFSET);

    if (value) {*dataReg |=  (1 << pin);} 
		else {*dataReg &= ~(1 << pin);}
}

//*****************************************************************************
// HAL_GPIO_ReadPin
//   - Return true if pin reads as 1, false if 0
//*****************************************************************************
bool HAL_GPIO_ReadPin(gpio_port port, uint8_t pin)
{
    uint32_t base = GPIO_PortBase(port);
    volatile uint32_t *dataReg = (volatile uint32_t *)(base + GPIO_DATA_OFFSET);

    // Return the pin state
    return ((*dataReg & (1 << pin)) != 0);
}

//*****************************************************************************
// Additional Helper: HAL_GPIO_DisablePin
//   - Disables (tri-states) the pin: sets as input, clears DEN, AFSEL, AMSEL, etc.
//*****************************************************************************

void gpio_start_interrupt(gpio_port port){
	
		switch(port)
		{
			case 0:
				(*((volatile unsigned long *)0xE000E100)) |= (1 << 0);
			 break;
			case 1:
				(*((volatile unsigned long *)0xE000E100)) |= (1 << 1);
				break;
			case 2:
				(*((volatile unsigned long *)0xE000E100)) |= (1 << 2);
				break;
			case 3:
				(*((volatile unsigned long *)0xE000E100)) |= (1 << 3);
				break;
			case 4:
				(*((volatile unsigned long *)0xE000E100)) |= (1 << 4);
				break;
			case 5:
				(*((volatile unsigned long *)0xE000E100)) |= (1 << 30);
				break;
		}
}

void gpio_disable_interrupt(gpio_port port){
		
		switch(port)
		{
			case 0:
				(*((volatile unsigned long *)0xE000E180)) |= (1 << 0);
			 break;
			case 1:
				(*((volatile unsigned long *)0xE000E180)) |= (1 << 1);
				break;
			case 2:
				(*((volatile unsigned long *)0xE000E180)) |= (1 << 2);
				break;
			case 3:
				(*((volatile unsigned long *)0xE000E180)) |= (1 << 3);
				break;
			case 4:
				(*((volatile unsigned long *)0xE000E180)) |= (1 << 4);
				break;
			case 5:
				(*((volatile unsigned long *)0xE000E180)) |= (1 << 30);
				break;
		}
}

void HAL_GPIO_DisablePin(const gpip_config *config)
{
		gpio_disable_interrupt(config->port);
    uint32_t base = GPIO_PortBase(config->port);

    // Direction to input
    volatile uint32_t *dirReg = (volatile uint32_t *)(base + GPIO_DIR_OFFSET);
    *dirReg &= ~(1 << config->pin_num);

    // Disable pull-up/down
    volatile uint32_t *purReg = (volatile uint32_t *)(base + GPIO_PUR_OFFSET);
    volatile uint32_t *pdrReg = (volatile uint32_t *)(base + GPIO_PDR_OFFSET);
    *purReg &= ~(1 << config->pin_num);
    *pdrReg &= ~(1 << config->pin_num);

    // Disable digital, AFSEL, and analog
    volatile uint32_t *denReg   = (volatile uint32_t *)(base + GPIO_DEN_OFFSET);
    volatile uint32_t *amselReg = (volatile uint32_t *)(base + GPIO_AMSEL_OFFSET);
    volatile uint32_t *afselReg = (volatile uint32_t *)(base + GPIO_AFSEL_OFFSET);

    *denReg   &= ~(1 << config->pin_num);
    *afselReg &= ~(1 << config->pin_num);
    *amselReg &= ~(1 << config->pin_num);
		
}

//*****************************************************************************
// HAL_GPIO_RegisterCallback
//*****************************************************************************
int HAL_GPIO_RegisterCallback(gpio_port port, uint8_t pin, gpio_callback_t callback, void *user_data)
{
    if (port > F || pin > 7) {
        return -1; // Invalid port or pin
    }

    gpio_callbacks[port][pin].callback = callback;
    gpio_callbacks[port][pin].user_data = user_data;

    // Enable GPIO interrupt for the pin
    uint32_t base = GPIO_PortBase(port);
    volatile uint32_t *imReg = (volatile uint32_t *)(base + 0x410); // GPIO_IM_OFFSET
    volatile uint32_t *isReg = (volatile uint32_t *)(base + 0x404);  //GPIO_IS_OFFSET
    volatile uint32_t *ibeReg = (volatile uint32_t *)(base + 0x408); //GPIO_IBE_OFFSET
    //volatile uint32_t *ievReg = (volatile uint32_t *)(base + GPIO_IEV_OFFSET); //GPIO_IEV_OFFSET
		
		*imReg &= ~(1 << pin);    // to prevent unwanted interrupts
    *isReg &= ~(1 << pin);   // Edge-triggered
    *ibeReg = 0x01;  // both edges
    //*ievReg |= (1 << pin);   // Rising edge trigger
    *imReg |= (1 << pin);    // Unmask interrupt		
    return 0;
}

//*****************************************************************************
// GPIO ISR Handler
//*****************************************************************************
static void GPIO_ISRHandler(gpio_port port)
{
    uint32_t base = GPIO_PortBase(port);
    volatile uint32_t *misReg = (volatile uint32_t *)(base + 0x418);  //GPIO_MIS_OFFSET
    volatile uint32_t *icrReg = (volatile uint32_t *)(base + 0x41C);  // GPIO_ICR_OFFSET

    uint32_t interrupt_status = *misReg;

    
        for (uint8_t pin = 0; pin < 8; ++pin) {
        if (interrupt_status & (1 << pin)) {
            if (gpio_callbacks[port][pin].callback) {
                gpio_callbacks[port][pin].callback(pin, gpio_callbacks[port][pin].user_data);
            }
            *icrReg |= (1 << pin); // Clear interrupt
        }
    }
    
}

//*****************************************************************************
// GPIO Port Handlers
//*****************************************************************************
void GPIOA_Handler(void) { GPIO_ISRHandler(A); }
//void GPIOB_Handler(void) { GPIO_ISRHandler(B); }
//void GPIOC_Handler(void) { GPIO_ISRHandler(C); }
void GPIOD_Handler(void) { GPIO_ISRHandler(D); }
void GPIOE_Handler(void) { GPIO_ISRHandler(E); }
void GPIOF_Handler(void) { GPIO_ISRHandler(F); }


void initButton(void)
{
	gpip_config pc4_config = {
    .pin_num = 4,
    .port = C,
    .PullUpR_Input = true,
    .PullDownR_Input = false,
    .InitialValue_Output = false, // Initial output value: 0
    .PinType = false,              // Pin type:	Input
    .PinProperty = true,          // Pin property: Digital
    .afselTim = false             // Alternate function: GPIO (no timer)
	};
	
	gpip_config pc5_config = {
    .pin_num = 5,
    .port = C,
    .PullUpR_Input = true,
    .PullDownR_Input = false,
    .InitialValue_Output = false, // Initial output value: 0
    .PinType = false,              // Pin type:	Input
    .PinProperty = true,          // Pin property: Digital
    .afselTim = false             // Alternate function: GPIO (no timer)
	};
	
	gpip_config pe5_config = {
    .pin_num = 5,
    .port = E,
    .PullUpR_Input = false,
    .PullDownR_Input = true,
    .InitialValue_Output = true, // Initial output value: 0
    .PinType = true,              // Pin type: Output
    .PinProperty = true,          // Pin property: Digital
    .afselTim = false             // Alternate function: GPIO (no timer)
};
	
	HAL_GPIO_Init(&pc4_config);	// Configure pc4 & 5 for detecting button presses
	HAL_GPIO_Init(&pc5_config);
	HAL_GPIO_Init(&pe5_config);
	
	GPIOC->IS &= ~0x30;   // Make PC4-5 edge-sensitive
  GPIOC->IBE &= ~0x30;  // Trigger on a single edge
  GPIOC->IEV &= ~0x30;  // Trigger on falling edge
  GPIOC->ICR |= 0x3F;   // Clear any prior interrupt on PC4-5
  GPIOC->IM |= 0x30;    // Enable interrupt for PC4-5
}
