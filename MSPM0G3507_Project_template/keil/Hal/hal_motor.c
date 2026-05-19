/**
 * hal_motor.c
 * Motor control abstraction layer implementation
 */
#include "hal_motor.h"
#include "hal_encode.h"
#include "hal_tb6612.h"
#include "hal_pid.h"

/* Left/right motor global instances */
Motor_t motor_left = {0};
Motor_t motor_right = {0};

void motor_init(void)
{
    motor_left.target = 0;
    motor_left.feedback = 0;
    motor_left.output = 0;
    pid_control_init(&motor_left.speed_ctrl,
                     1.0f, 0.0f, 0.0f,  /* kp, ki, kd */
                     100.0f,              /* err_max */
                     3000.0f,             /* integral_max */
                     500.0f,              /* output_max */
                     0,                   /* err_limit_flag */
                     0,                   /* integral_separate_flag */
                     0,                   /* integral_separate_limit */
                     6);                  /* dis_error_gap_cnt */

    motor_right.target = 0;
    motor_right.feedback = 0;
    motor_right.output = 0;
    pid_control_init(&motor_right.speed_ctrl,
                     1.0f, 0.0f, 0.0f,  /* kp, ki, kd */
                     100.0f,              /* err_max */
                     3000.0f,             /* integral_max */
                     500.0f,              /* output_max */
                     0,                   /* err_limit_flag */
                     0,                   /* integral_separate_flag */
                     0,                   /* integral_separate_limit */
                     6);                  /* dis_error_gap_cnt */
}

void motor_set_target(float left, float right)
{
    motor_left.target = left;
    motor_right.target = right;
}

void motor_control(void)
{
    /* Update feedback from encoder */
    motor_left.feedback = smartcar_imu.left_motor_speed_cmps;
    motor_right.feedback = smartcar_imu.right_motor_speed_cmps;

    /* Left motor PID */
    motor_left.speed_ctrl.expect = motor_left.target;
    motor_left.speed_ctrl.feedback = motor_left.feedback;
    motor_left.output = pid_control_run(&motor_left.speed_ctrl);

    /* Right motor PID */
    motor_right.speed_ctrl.expect = motor_right.target;
    motor_right.speed_ctrl.feedback = motor_right.feedback;
    motor_right.output = pid_control_run(&motor_right.speed_ctrl);
}

void motor_set_pwm(int left, int right)
{
    Set_Pwm(left, right, 0, 0);
}

float motor_get_left_speed(void)
{
    return smartcar_imu.left_motor_speed_cmps;
}

float motor_get_right_speed(void)
{
    return smartcar_imu.right_motor_speed_cmps;
}