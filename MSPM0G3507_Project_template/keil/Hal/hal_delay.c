
#include "hal_delay.h"


volatile unsigned int delay_times = 0;

// Use SysTick timer for precise us delay
void hal_delay_us(uint32_t us)
{
    delay_times = us;
    while( delay_times != 0 );
}

// Use SysTick timer for precise ms delay
void hal_delay_ms(uint16_t ms) 
{
    delay_times = ms*1000;
    while( delay_times != 0 );
}


// SysTick interrupt handler
void SysTick_Handler(void)
{
    if( delay_times != 0 )
    {
        delay_times--;
    }
		
		
		
}


//// Microsecond software delay (commented out)
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
//// Millisecond software delay (commented out)
//void hal_delay_ms(uint16_t delay_ms)
//{    
//  volatile unsigned int num;
//  for (num = 0; num < delay_ms; num++)
//  {
//    hal_delay_us(1000);
//  }
//}


//// Custom delay with rough timing (commented out)
//void delay_ms(unsigned int ms)
//{
//    unsigned int i, j;
//    // Loop count roughly calculated based on CPU frequency and compiler-generated cycles
//    // Adjust through actual testing to achieve target delay
//    for (i = 0; i < ms; i++)
//    {
//        for (j = 0; j < 8000; j++)
//        {
//            // Execute predictable instruction cycles for timing
//            __asm__("nop"); // "nop" is a no-operation instruction, takes one or a few clock cycles
//        }
//    }
//}