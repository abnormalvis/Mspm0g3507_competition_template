#ifndef _HAL_DELAY_H
#define _HAL_DELAY_H
#include "stdint.h"

void hal_delay_us(uint32_t delay_us);
void hal_delay_ms(uint16_t delay_ms);

// For get_clock_ms() compatibility in Delay.c
extern volatile uint32_t tick_ms;

#endif
