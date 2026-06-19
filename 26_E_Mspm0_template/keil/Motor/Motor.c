#include "ti_msp_dl_config.h"
#define max_duty 10000.0f
void Motor_SetPWML(float pwm)
{
    if (pwm > 0 && pwm <= max_duty)
    {
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Left_A_PIN);
        DL_GPIO_clearPins(MOTOR_DIR_PORT, MOTOR_DIR_Left_B_PIN);
        DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, pwm, GPIO_PWM_Motor_C3_IDX);
    }
    else if (pwm >= -max_duty && pwm < 0)
    {
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Left_B_PIN);
        DL_GPIO_clearPins(MOTOR_DIR_PORT, MOTOR_DIR_Left_A_PIN);
        DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, -pwm, GPIO_PWM_Motor_C3_IDX);
    }
    else
    {
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Left_A_PIN | MOTOR_DIR_Left_B_PIN);
        DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C3_IDX);
    }
}

void Motor_SetPWMR(float pwm)
{
    if (pwm > 0 && pwm <= max_duty)
    {
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Right_A_PIN);
        DL_GPIO_clearPins(MOTOR_DIR_PORT, MOTOR_DIR_Right_B_PIN);
        DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, pwm, GPIO_PWM_Motor_C1_IDX);
    }
    else if (pwm >= -max_duty && pwm < 0)
    {
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Right_B_PIN);
        DL_GPIO_clearPins(MOTOR_DIR_PORT, MOTOR_DIR_Right_A_PIN);
        DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, -pwm, GPIO_PWM_Motor_C1_IDX);
    }
    else
    {
        DL_GPIO_setPins(MOTOR_DIR_PORT, MOTOR_DIR_Right_A_PIN | MOTOR_DIR_Right_B_PIN);
        DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C1_IDX);
    }
}
