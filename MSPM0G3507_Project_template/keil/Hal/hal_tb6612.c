#include "ti_msp_dl_config.h"
#include "ti/driverlib/dl_gpio.h"
#include "hal_tb6612.h"
#include <ti/driverlib/dl_timera.h>

/* Usage: Left motor forward
 * Input params: 
     * PWM_Hight: target high-level pulse width
 * Return params: None
*/
void Motor_Foreward_Left(uint16_t PWM_Hight)
{
	BIN1_OUT(0);
	BIN2_OUT(1);		

	DL_TimerA_setCaptureCompareValue(PWM_0_INST,PWM_Hight,GPIO_PWM_0_C3_IDX);

}
/* Usage: Left motor reverse
 * Input params: 
     * PWM_Hight: target high-level pulse width
 * Return params: None
*/
void Motor_Backward_Left(uint16_t PWM_Hight)
{
	BIN1_OUT(1);
	BIN2_OUT(0);
	DL_TimerA_setCaptureCompareValue(PWM_0_INST,PWM_Hight,GPIO_PWM_0_C3_IDX);
}

/* Usage: Right motor forward
 * Input params: 
     * PWM_Hight: target high-level pulse width, unit 0.8us
 * Return params: None
*/
void Motor_Foreward_Right(uint16_t PWM_Hight)
{  
	AIN1_OUT(1);
	AIN2_OUT(0);
	DL_TimerA_setCaptureCompareValue(PWM_0_INST,PWM_Hight,GPIO_PWM_0_C1_IDX);
}

/* Usage: Right motor reverse
 * Input params: 
     * PWM_Hight: target high-level pulse width, unit 0.8us
 * Return params: None
*/
void Motor_Backward_Right(uint16_t PWM_Hight)
{
	AIN1_OUT(0);
	AIN2_OUT(1);
	DL_TimerA_setCaptureCompareValue(PWM_0_INST,PWM_Hight,GPIO_PWM_0_C1_IDX);
}

/* Usage: Stop all motors
 * Input params: None
 * Return params: None
*/
void Motor_Stop_All(void)
{
	AIN1_OUT(0);
	AIN2_OUT(0);
	BIN1_OUT(0);
	BIN2_OUT(0);
	DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C1_IDX);
	DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C3_IDX);
}
