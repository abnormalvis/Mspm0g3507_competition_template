#ifndef __DELAY_H__
#define __DELAY_H__

// 若需要获取毫秒计数（可选，基于全局变量）
extern volatile uint32_t tick_ms;
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);
// 简化版获取时钟（基于Delay_ms的计数模拟）
int get_clock_ms(uint32_t *count);
bool State_Delay(uint32_t T_cycle_ms , uint8_t Flag , uint8_t Flag_State , uint32_t Delay_ms );

#endif
