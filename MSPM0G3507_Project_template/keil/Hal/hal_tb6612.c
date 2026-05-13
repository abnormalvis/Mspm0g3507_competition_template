#include "ti_msp_dl_config.h"
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
	
	DL_TimerG_setCaptureCompareValue(PWM_0_INST,PWM_Hight,GPIO_PWM_0_C3_IDX); 
	
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
	DL_TimerG_setCaptureCompareValue(PWM_0_INST,PWM_Hight,GPIO_PWM_0_C3_IDX);	                
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
	DL_TimerG_setCaptureCompareValue(PWM_0_INST,PWM_Hight,GPIO_PWM_0_C1_IDX);
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
	DL_TimerG_setCaptureCompareValue(PWM_0_INST,PWM_Hight,GPIO_PWM_0_C1_IDX);
}
