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

    DL_TimerA_setCaptureCompareValue(PWM_0_INST, PWM_Hight, GPIO_PWM_0_C3_IDX);
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
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, PWM_Hight, GPIO_PWM_0_C3_IDX);
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
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, PWM_Hight, GPIO_PWM_0_C1_IDX);
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
    DL_TimerA_setCaptureCompareValue(PWM_0_INST, PWM_Hight, GPIO_PWM_0_C1_IDX);
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

/* Set PWM for all 4 channels - matching reference project interface
 * a: left motor forward PWM
 * b: left motor backward PWM
 * c: right motor forward PWM
 * d: right motor backward PWM */
void Set_Pwm(int a, int b, int c, int d)
{
    if (a > 0)
    {
        AIN1_OUT(0);
        AIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)a, GPIO_PWM_0_C3_IDX);
    }
    else if (a < 0)
    {
        AIN1_OUT(1);
        AIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-a), GPIO_PWM_0_C3_IDX);
    }
    else
    {
        AIN1_OUT(0);
        AIN2_OUT(0);
    }

    if (b > 0)
    {
        BIN1_OUT(0);
        BIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)b, GPIO_PWM_0_C1_IDX);
    }
    else if (b < 0)
    {
        BIN1_OUT(1);
        BIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-b), GPIO_PWM_0_C1_IDX);
    }
    else
    {
        BIN1_OUT(0);
        BIN2_OUT(0);
    }

    if (c > 0)
    {
        AIN1_OUT(0);
        AIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)c, GPIO_PWM_0_C3_IDX);
    }
    else if (c < 0)
    {
        AIN1_OUT(1);
        AIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-c), GPIO_PWM_0_C3_IDX);
    }
    else
    {
        AIN1_OUT(0);
        AIN2_OUT(0);
    }

    if (d > 0)
    {
        BIN1_OUT(0);
        BIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)d, GPIO_PWM_0_C1_IDX);
    }
    else if (d < 0)
    {
        BIN1_OUT(1);
        BIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-d), GPIO_PWM_0_C1_IDX);
    }
    else
    {
        BIN1_OUT(0);
        BIN2_OUT(0);
    }
}

/* Debug: direct PWM output bypassing PID and direction logic
 * left_duty/right_duty: positive=forward, negative=backward, 0=stop */
void Set_Pwm_Debug(int left_duty, int right_duty)
{
    if (left_duty > 0)
    {
        AIN1_OUT(0);
        AIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)left_duty, GPIO_PWM_0_C3_IDX);
    }
    else if (left_duty < 0)
    {
        AIN1_OUT(1);
        AIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-left_duty), GPIO_PWM_0_C3_IDX);
    }
    else
    {
        AIN1_OUT(0);
        AIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C3_IDX);
    }

    if (right_duty > 0)
    {
        BIN1_OUT(0);
        BIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)right_duty, GPIO_PWM_0_C1_IDX);
    }
    else if (right_duty < 0)
    {
        BIN1_OUT(1);
        BIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, (uint16_t)(-right_duty), GPIO_PWM_0_C1_IDX);
    }
    else
    {
        BIN1_OUT(0);
        BIN2_OUT(0);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C1_IDX);
    }
}


/* 参考
#include "motor.h"
//1为正转 -1为反转
int8_t motorL_dir = 1;
int8_t motorR_dir = 1;
// 绝对值函数
int myabs(int a)
{
    return (a >= 0) ? a : -a;
}
// 速度限制函数
void Limit(int *motor_speed)
{
    if (*motor_speed > PWM_MAX)
        *motor_speed = PWM_MAX;
    if (*motor_speed < PWM_MIN)
        *motor_speed = PWM_MIN;
}

//整体赋值
void Set_Speed(int motor_l, int motor_r)
{
    // 限制速度范围
    Limit(&motor_l);
		Limit(&motor_r);
    // 左电机控制 (PB11, PB12)
    if (motor_l >= 0)
    {
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_SET);   
        __HAL_TIM_SET_COMPARE(&htim2, MOTOR_L_PWM_CHANNEL, motor_l);
    }
    else
    {
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_SET);   
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim2, MOTOR_L_PWM_CHANNEL, myabs(motor_l));
    }

    // 右电机控制 (PB2, PB10)
    if (motor_r >= 0)
    {
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_SET);  
        __HAL_TIM_SET_COMPARE(&htim2, MOTOR_R_PWM_CHANNEL, motor_r);
    }
    else
    {
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_SET);  
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim2, MOTOR_R_PWM_CHANNEL, myabs(motor_r));
    }
}

//单独赋值
void Set_MotorL_Speed(int motor_l)
{
		Limit(&motor_l);
    // 左电机控制
    if (motor_l >= 0)
    {
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_SET);   
        __HAL_TIM_SET_COMPARE(&htim2, MOTOR_L_PWM_CHANNEL, motor_l);
    }
    else
    {
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_L_IN1_PIN, GPIO_PIN_SET);   
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_L_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim2, MOTOR_L_PWM_CHANNEL, myabs(motor_l));
    }
}

void Set_MotorR_Speed(int motor_r)
{
		Limit(&motor_r);
    // 右电机控制
    if (motor_r >= 0)
    {
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_SET);  
        __HAL_TIM_SET_COMPARE(&htim2, MOTOR_R_PWM_CHANNEL, motor_r);
    }
    else
    {
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_R_IN1_PIN, GPIO_PIN_SET);  
        HAL_GPIO_WritePin(MOTOR_GPIO_PORT, MOTOR_R_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim2, MOTOR_R_PWM_CHANNEL, myabs(motor_r));
    }
}
    */