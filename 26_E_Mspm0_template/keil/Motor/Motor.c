#include "ti_msp_dl_config.h"
#include "Motor.h"
#define max_duty 10000.0f

#ifdef MOTOR_DRIVER_C107A

/* ===================================================================
 * C107A / TB6612 Implementation
 *   - Uses direction GPIO pins (PA24, PA25, PA26, PA22)
 *   - PWM channels: C1 (right), C3 (left) on TIMA0
 * =================================================================== */

void Motor_Init(void)
{
    /* Set all direction pins HIGH, PWM duty = 0 -> brake state */
    DL_GPIO_setPins(MOTOR_DIR_PORT,
        MOTOR_DIR_Left_A_PIN | MOTOR_DIR_Left_B_PIN |
        MOTOR_DIR_Right_A_PIN | MOTOR_DIR_Right_B_PIN);
    DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C3_IDX);
    DL_TimerG_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C1_IDX);
}

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

#elif defined(MOTOR_DRIVER_AT8236)

/* ===================================================================
 * AT8236 Implementation
 *   - Complementary PWM scheme (no direction GPIO pins needed)
 *   - Channel mapping:
 *       Left motor:  IN1=TIMA0_C3(PA23), IN2=TIMA0_C0(PB8)
 *       Right motor: IN1=TIMA0_C1(PA7),  IN2=TIMA0_C2(PB0)
 *   - PWM mode: edge-aligned up, init_val=LOW
 *       CC=0      -> 100% duty (output always HIGH)
 *       CC=period ->   0% duty (output always LOW)
 *   - Scheme:
 *       Forward (pwm>0): IN1=0(100%), IN2=pwm
 *       Reverse (pwm<0): IN1=-pwm,     IN2=0(100%)
 *       Brake   (pwm=0): IN1=0,        IN2=0 (both 100% -> brake)
 * =================================================================== */

void Motor_Init(void)
{
    /* Brake state: all four channels at 100% duty (CC=0) */
    DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C3_IDX); /* Left  IN1 */
    DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C0_IDX); /* Left  IN2 */
    DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C1_IDX); /* Right IN1 */
    DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C2_IDX); /* Right IN2 */
}

void Motor_SetPWML(float pwm)
{
    if (pwm > 0 && pwm <= max_duty)
    {
        /* Forward: IN1=100% duty, IN2=reduced duty */
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C3_IDX);
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, (uint32_t)pwm, GPIO_PWM_Motor_C0_IDX);
    }
    else if (pwm >= -max_duty && pwm < 0)
    {
        /* Reverse: IN1=reduced duty, IN2=100% duty */
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, (uint32_t)(-pwm), GPIO_PWM_Motor_C3_IDX);
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C0_IDX);
    }
    else
    {
        /* Brake: both channels 100% duty */
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C3_IDX);
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C0_IDX);
    }
}

void Motor_SetPWMR(float pwm)
{
    if (pwm > 0 && pwm <= max_duty)
    {
        /* Forward: IN1=100% duty, IN2=reduced duty */
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C1_IDX);
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, (uint32_t)pwm, GPIO_PWM_Motor_C2_IDX);
    }
    else if (pwm >= -max_duty && pwm < 0)
    {
        /* Reverse: IN1=reduced duty, IN2=100% duty */
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, (uint32_t)(-pwm), GPIO_PWM_Motor_C2_IDX);
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C1_IDX);
    }
    else
    {
        /* Brake: both channels 100% duty */
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C1_IDX);
        DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, 0, GPIO_PWM_Motor_C2_IDX);
    }
}

#else
#error "No motor driver selected. Define MOTOR_DRIVER_C107A or MOTOR_DRIVER_AT8236 in Motor.h"
#endif
