#ifndef HAL_GPTIMER_H
#define HAL_GPTIMER_H

#include <stdbool.h>
#include <stdint.h>


typedef enum {
    GPTIMER_ONE_SHOT = 0, 
    GPTIMER_PERIODIC = 1, 
    GPTIMER_EDGE_TIME = 2, 
    GPTIMER_EDGE_COUNT = 3 
} gptimer_mode_t;

typedef enum {
    GPTIMER_POSITIVE_EDGE = 0, 
    GPTIMER_NEGATIVE_EDGE = 1, 
    GPTIMER_BOTH_EDGE = 3,     
} gptimer_count_mode_t;

typedef struct {
    uint8_t timer_num;      /*!< Number of the timer */
    bool wide_timer;        /*!< Select wide timer (32/64-bit) */
    gptimer_mode_t mode;    /*!< Timer operation mode */
    bool count_up;          /*!< Count direction: true for up, false for down */
    uint8_t prescaler;      /*!< Prescaler value */
    uint32_t load_value;    /*!< Initial load value */
		gptimer_count_mode_t count_mode;
} gptimer_config_t;

typedef void (*gptimer_callback_t)(uint32_t count_value, void *user_data);

int gptimer_init(const gptimer_config_t *config);

int gptimer_start(uint8_t timer_num);

int gptimer_stop(uint8_t timer_num);

int gptimer_set_load(uint8_t timer_num, uint32_t load_value);

int gptimer_get_value(uint8_t timer_num, uint32_t *current_value);

int gptimer_register_callback(uint8_t timer_num, gptimer_callback_t callback, void *user_data);

#endif // HAL_GPTIMER_H