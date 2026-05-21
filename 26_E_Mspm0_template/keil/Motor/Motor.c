#include "ti_msp_dl_config.h"


void Motor_SetPWML(float pwm){
	if(pwm<=3200&&pwm>=0){
        DL_GPIO_clearPins(MOTOR_DIR_PORT, MOTOR_DIR_Left_B_PIN);
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Left_A_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, pwm, GPIO_PWM_Motor_C1_IDX);
	}
    if(pwm>=-3200&&pwm<=0){
        DL_GPIO_clearPins(MOTOR_DIR_PORT, MOTOR_DIR_Left_A_PIN);
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Left_B_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, -pwm, GPIO_PWM_Motor_C1_IDX);
	}
}

void Motor_SetPWMR(float pwm){
	if(pwm<=3200&&pwm>=0){
        DL_GPIO_clearPins(MOTOR_DIR_PORT, MOTOR_DIR_Right_A_PIN);
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Right_B_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, pwm, GPIO_PWM_Motor_C3_IDX);
	}
    if(pwm>=-3200&&pwm<=0){
        DL_GPIO_clearPins(MOTOR_DIR_PORT, MOTOR_DIR_Right_B_PIN);
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Right_A_PIN);
		DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, -pwm, GPIO_PWM_Motor_C3_IDX);
	}
}
