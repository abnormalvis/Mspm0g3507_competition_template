#include "ti/driverlib/dl_gpio.h"
#include "hal_tb6612.h"
#include <ti/driverlib/dl_timerg.h>

/* 作用 ： 左电机正转
 * 传入参数 ： 
     * PWM_Hight ： 设置需要输出的高电平脉宽
 * 返回参数 ： 无
*/
void Motor_Foreward_Left(uint16_t PWM_Hight)
{ 
	BIN1_OUT(0);
	BIN2_OUT(1);		
	
	DL_TimerG_setCaptureCompareValue(TIMA0,PWM_Hight,DL_TIMER_CC_3_INDEX); 
	
}
/* 作用 ： 左电机反转
 * 传入参数 ： 
     * PWM_Hight ： 设置需要输出的高电平脉宽
 * 返回参数 ： 无
*/
void Motor_Backward_Left(uint16_t PWM_Hight)
{
	BIN1_OUT(1);
	BIN2_OUT(0);
	DL_TimerG_setCaptureCompareValue(TIMA0,PWM_Hight,DL_TIMER_CC_3_INDEX);	                
}

/* 作用 ： 右电机正转
 * 传入参数 ： 
     * PWM_Hight ： 设置需要输出的高电平脉宽，单位0.8us
 * 返回参数 ： 无
*/
void Motor_Foreward_Right(uint16_t PWM_Hight)
{  
	AIN1_OUT(1);
	AIN2_OUT(0);
	DL_TimerG_setCaptureCompareValue(TIMA0,PWM_Hight,DL_TIMER_CC_1_INDEX);
}

/* 作用 ： 右电机反转
 * 传入参数 ： 
     * PWM_Hight ： 设置需要输出的高电平脉宽，单位0.8us
 * 返回参数 ： 无
*/
void Motor_Backward_Right(uint16_t PWM_Hight)
{
	AIN1_OUT(0);
	AIN2_OUT(1);
	DL_TimerG_setCaptureCompareValue(TIMA0,PWM_Hight,DL_TIMER_CC_1_INDEX);
}
