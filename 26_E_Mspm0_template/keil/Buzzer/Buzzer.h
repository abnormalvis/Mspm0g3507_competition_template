#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "ti_msp_dl_config.h"
#include "Delay.h"

extern bool beep_on_1s_flag;

void Buzzer_on();
void Buzzer_off();
//放1ms中断,若beef_on_1s_flag=1,蜂鸣器响
//注意若使用此函数则上面的函数无效
void Buzzer_on_1s(uint32_t TimerCount);

#endif
