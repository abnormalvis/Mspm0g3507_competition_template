#ifndef __Key_H__
#define __Key_H__

#include "ti_msp_dl_config.h"
//共阴按键，上拉输入
//存储键码值，按键抬起键码值+1
extern uint8_t Key_Num[10];
//此函数放20ms中断
void Get_Key(void);

#endif

