#ifndef __DELAY_H__
#define __DELAY_H__

#include "hal_delay.h"

void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);
int get_clock_ms(uint32_t *count);
bool State_Delay(uint32_t T_cycle_ms , uint8_t Flag , uint8_t Flag_State , uint32_t Delay_ms );

#endif
