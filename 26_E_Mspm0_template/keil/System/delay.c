#include "delay.h"

/* ================ 延时函数封装 =================== */

void delay_us(int __us) { delay_cycles( (CPUCLK_FREQ / 1000 / 1000)*__us); }
void delay_ms(int __ms) { delay_cycles( (CPUCLK_FREQ / 1000)*__ms); }
/******************延时*************************************/
volatile uint32_t delay_times = 0;
void SysTick_Handler(void)
{
    if (delay_times != 0)
    {
        delay_times--;
    }
}
// 搭配滴答定时器实现的精确ms延时
void Delay_Ms(unsigned int ms)
{
    delay_times = ms;
    while (delay_times != 0);
}

//void abs(int *param) 
//{


//    // 如果指向的数值是负数，则转换为其相反数（绝对值）
//    if (*param < 0) {
//        *param = -(*param);
//    }
//    // 非负数不做处理（本身就是绝对值）
//}


