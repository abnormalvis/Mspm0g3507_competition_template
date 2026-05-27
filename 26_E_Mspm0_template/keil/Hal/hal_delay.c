#include "hal_delay.h"

volatile unsigned int delay_times = 0;

// For get_clock_ms() compatibility in Delay.c
volatile uint32_t tick_ms = 0;

// SysTick period is 400us; use divisor for correct timing
void hal_delay_us(uint32_t us)
{
    delay_times = (us + 399) / 400;
    while( delay_times != 0 );
}

// 2.5 SysTick ticks per ms (1000us / 400us)
void hal_delay_ms(uint16_t ms)
{
    delay_times = ((uint32_t)ms * 5) / 2;
    while( delay_times != 0 );
}

// SysTick interrupt handler -- serves both hal_delay and tick_ms
void SysTick_Handler(void)
{
    tick_ms++;
    if( delay_times != 0 )
    {
        delay_times--;
    }
}
