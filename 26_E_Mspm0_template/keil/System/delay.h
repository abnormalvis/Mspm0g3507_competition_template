#ifndef __DELAY_H__
#define __DELAY_H__

#include "headfile.h"

/* 延时函数 */
//适合短延时
void delay_us(int __us);
void delay_ms(int __ms);
//适合长延时(上百毫秒，但是不能重复调用)
void Delay_Ms(unsigned int ms);
#endif
