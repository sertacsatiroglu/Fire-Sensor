#include <stdbool.h>
#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "gptimer.h"

#define TIMER_REGISTER_BASE 0x40030000
#define TIMERX_REGISTER_BASE(x) (0x40030000 |  (x << 12))

#define PWMX_REGISTER_BASE(x) ((x) == 0 ? (0x40028000) : (0x40029000))
#define PMWX_OFFSET(x,offset) ((x) == 0 ? (offset) : (offset+0x040))

// Internal structure to hold registered callbacks
typedef struct {
    gptimer_callback_t callback;
    void *user_data;
} gptimer_callback_info_t;

gptimer_callback_info_t gptimer_callbacks[6]; // Assuming 6 timers for simplicity

int gptimer_init(const gptimer_config_t *config) {
    if (!config) {
        return -1;
    }
		
		if(config->timer_num > 5)
		{
			return -1;
		}
		
		SYSCTL->RCGCTIMER |= 1 << config->timer_num;

    // Wait for the timer to be ready
    while ((SYSCTL->PRTIMER & (1 << config->timer_num)) == 0);

    // Disable the timer before configuration
    (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x0000000C))) = 0x00000000;

    // Configure the timer as wide or regular
		(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000000))) = config->wide_timer ? 0x00000000 : 0x00000004 ;
		
    // Set the mode
		switch(config->mode)
		{
			case GPTIMER_ONE_SHOT:
				(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000004))) = 0x00000001;
				(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000018))) = 0x00000001;  // ISR Time-Out Interrupt Mask
				break;
			case GPTIMER_PERIODIC:
				(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000004))) = 0x00000002;
				(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000018))) = 0x00000001;	// ISR Time-Out Interrupt Mask
				break;
			case GPTIMER_EDGE_TIME:
				(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000004))) = (0x00000003 | 0x00000004);
			  (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x0000000C))) |= ((uint32_t) config->count_mode) << 2;
				(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000018))) = 0x00000004 | 0x00000001;  // ISR A Capture Mode Event Interrupt Mask
				break;
			case GPTIMER_EDGE_COUNT:
				(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000004))) = (0x00000003 | 0x00000000);
				(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x0000000C))) |= ((uint32_t) config->count_mode) << 2;
				(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000018))) = 0x00000004 | 0x00000001;  // ISR A Capture Mode Event Interrupt Mask
				break;
			default:
				return -1;
		}

		
    // Set the count direction
    if (config->count_up) {
        (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000004))) |= 0x00000010;
    }

    // Configure prescaler
		(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000038))) = (uint32_t) config->prescaler;

    // Load the initial value
		(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(config->timer_num) + 0x00000028))) = config->load_value;
		
    return 0;
}

int gptimer_start(uint8_t timer_num) {
		(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(timer_num) + 0x0000000C))) |= 0x00000001;
	
		switch(timer_num)
		{
			case 0:
				(*((volatile unsigned long *)0xE000E100)) |= (1 << 19);
			 break;
			case 1:
				(*((volatile unsigned long *)0xE000E100)) |= (1 << 21);
				break;
			case 2:
				(*((volatile unsigned long *)0xE000E100)) |= (1 << 23);
				break;
			case 3:
				(*((volatile unsigned long *)0xE000E104)) |= (1 << 3);
				break;
			case 4:
				(*((volatile unsigned long *)0xE000E108)) |= (1 << 6);
				break;
			case 5:
				(*((volatile unsigned long *)0xE000E108)) |= (1 << 28);
				break;
		}
		
    return 0;
}

int gptimer_stop(uint8_t timer_num) {
	(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(timer_num) + 0x0000000C))) &= ~0x00000001;

			switch(timer_num)
		{
			case 0:
				(*((volatile unsigned long *)0xE000E180)) |= (1 << 19);
			 break;
			case 1:
				(*((volatile unsigned long *)0xE000E180)) |= (1 << 21);
				break;
			case 2:
				(*((volatile unsigned long *)0xE000E180)) |= (1 << 23);
				break;
			case 3:
				(*((volatile unsigned long *)0xE000E184)) |= (1 << 3);
				break;
			case 4:
				(*((volatile unsigned long *)0xE000E188)) |= (1 << 6);
				break;
			case 5:
				(*((volatile unsigned long *)0xE000E188)) |= (1 << 28);
				break;
		}
		
    return 0;
}

int gptimer_set_load(uint8_t timer_num, uint32_t load_value) 
{
	(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(timer_num) + 0x00000028))) = load_value;
    return 0;
}

int gptimer_get_value(uint8_t timer_num, uint32_t *current_value) {
    if (!current_value) {
        return -1;
    }
    *current_value = (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(timer_num) + 0x00000048)));
    return 0;
}

int gptimer_register_callback(uint8_t timer_num, gptimer_callback_t callback, void *user_data) {

	  gptimer_callbacks[timer_num].callback = callback;
    gptimer_callbacks[timer_num].user_data = user_data;
    return 0;
}

static void gptimer_clear_interrupt(uint8_t timer_num, uint32_t interrupt_status) {
		(*((volatile uint32_t *)(TIMERX_REGISTER_BASE(timer_num) + 0x00000024))) |= interrupt_status;
}

static void TimerISRHandler(uint8_t timer_num) {
    int timer_count = (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(timer_num) + 0x00000048)));

    if (gptimer_callbacks[timer_num].callback) {
        gptimer_callbacks[timer_num].callback(timer_count, gptimer_callbacks[timer_num].user_data);
    }
}

void TIMER0A_Handler(void)
{
	uint32_t masked_interrupt_status = (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(0) + 0x00000020)));
	if(masked_interrupt_status)
	{
		TimerISRHandler(0);
		gptimer_clear_interrupt(0, masked_interrupt_status);
	}
}

void TIMER1A_Handler(void)
{
	uint32_t masked_interrupt_status = (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(1) + 0x00000020)));
	if(masked_interrupt_status)
	{
		TimerISRHandler(1);
		gptimer_clear_interrupt(1, masked_interrupt_status);
	}
}

void TIMER2A_Handler(void)
{
	uint32_t masked_interrupt_status = (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(2) + 0x00000020)));
	if(masked_interrupt_status)
	{
		TimerISRHandler(2);
		gptimer_clear_interrupt(2, masked_interrupt_status);
	}
}

void TIMER3A_Handler(void)
{
	uint32_t masked_interrupt_status = (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(3) + 0x00000020)));
	if(masked_interrupt_status)
	{
		TimerISRHandler(3);
		gptimer_clear_interrupt(3, masked_interrupt_status);
	}
}

void TIMER4A_Handler(void)
{
	uint32_t masked_interrupt_status = (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(4) + 0x00000020)));
	if(masked_interrupt_status)
	{
		TimerISRHandler(4);
		gptimer_clear_interrupt(4, masked_interrupt_status);
	}
}

void TIMER5A_Handler(void)
{
	uint32_t masked_interrupt_status = (*((volatile uint32_t *)(TIMERX_REGISTER_BASE(5) + 0x00000020)));
	if(masked_interrupt_status)
	{
		TimerISRHandler(5);
		gptimer_clear_interrupt(5, masked_interrupt_status);
	}
}

