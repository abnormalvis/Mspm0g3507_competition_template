#include "hal_motor.h"
#include "hal_encode.h"
#include "hal_tb6612.h"
#include "hal_pid.h"

void Pid_Motor_Control(void)
{
    uint8_t left_dir, right_dir;

    /* Always sample encoder deltas so feedback reflects current speed */
    Encoder_Update(&motor_left_encoder);
    Encoder_Update(&motor_right_encoder);

    /* --- Left motor (B channels) --- */
    if (motorL.target == 0) {
        BIN1_OUT(1); BIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C3_IDX);
        motorL.out = 0;
    } else {
        left_dir = (motorL.target > 0) ? 1 : 0;
        if (motorL.target < 0) motorL.target = -motorL.target;

        motorL.now = (float)Get_Encoder_Count(&motor_left_encoder);
        if (motorL.now < 0) motorL.now = -motorL.now;

        Pid_Cal(&motorL);
        Set_MotorL_Speed(left_dir, (uint32_t)motorL.out);
    }

    /* --- Right motor (A channels) --- */
    if (motorR.target == 0) {
        AIN1_OUT(1); AIN2_OUT(1);
        DL_TimerA_setCaptureCompareValue(PWM_0_INST, 0, GPIO_PWM_0_C1_IDX);
        motorR.out = 0;
    } else {
        right_dir = (motorR.target > 0) ? 1 : 0;
        if (motorR.target < 0) motorR.target = -motorR.target;

        motorR.now = (float)Get_Encoder_Count(&motor_right_encoder);
        if (motorR.now < 0) motorR.now = -motorR.now;

        Pid_Cal(&motorR);
        Set_MotorR_Speed(right_dir, (uint32_t)motorR.out);
    }
}
