#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "ti_msp_dl_config.h"
#include "Delay.h"

extern bool beep_on_1s_flag;

/* ---- Polarity: uncomment if your buzzer is active-HIGH ---- */
// #define BUZZER_ACTIVE_HIGH

#ifdef BUZZER_ACTIVE_HIGH
    #define BUZZER_ON()  Buzzer_off()   /* HIGH = on  */
    #define BUZZER_OFF() Buzzer_on()    /* LOW  = off */
#else
    #define BUZZER_ON()  Buzzer_on()    /* LOW  = on  (default active-low) */
    #define BUZZER_OFF() Buzzer_off()   /* HIGH = off (default active-low) */
#endif

void Buzzer_Init(void);
void Buzzer_on(void);
void Buzzer_off(void);
//放1ms中断,若beef_on_1s_flag=1,蜂鸣器响
//注意若使用此函数则上面的函数无效
void Buzzer_on_1s(uint32_t TimerCount);

#endif
