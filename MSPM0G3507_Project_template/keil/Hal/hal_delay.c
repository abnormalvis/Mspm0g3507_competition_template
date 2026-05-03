
#include "hal_delay.h"


volatile unsigned int delay_times = 0;

//搭配滴答定时器实现的精确ms延时
void hal_delay_us(uint32_t us)
{
    delay_times = us;
    while( delay_times != 0 );
}

//搭配滴答定时器实现的精确ms延时
void hal_delay_ms(uint16_t ms) 
{
    delay_times = ms*1000;
    while( delay_times != 0 );
}


//滴答定时器中断服务函数
void SysTick_Handler(void)
{
    if( delay_times != 0 )
    {
        delay_times--;
    }
		
		
		
}


////微秒级的延时
//void hal_delay_us(uint32_t delay_us)
//{    
//  volatile unsigned int num;
//  volatile unsigned int t;
// 
//  
//  for (num = 0; num < delay_us; num++)
//  {
//    t = 8;
//    while (t != 0)
//    {
//      t--;
//    }
//  }
//}
////毫秒级的延时
//void hal_delay_ms(uint16_t delay_ms)
//{    
//  volatile unsigned int num;
//  for (num = 0; num < delay_ms; num++)
//  {
//    hal_delay_us(1000);
//  }
//}


////自定义延时（不精确）
//void delay_ms(unsigned int ms)
//{
//    unsigned int i, j;
//    // 下面的嵌套循环的次数是根据主控频率和编译器生成的指令周期大致计算出来的，
//    // 需要通过实际测试调整来达到所需的延时。
//    for (i = 0; i < ms; i++)
//    {
//        for (j = 0; j < 8000; j++)
//        {
//            // 仅执行一个足够简单以致于可以预测其执行时间的操作
//            __asm__("nop"); // "nop" 代表“无操作”，在大多数架构中，这会消耗一个或几个时钟周期
//        }
//    }
//}