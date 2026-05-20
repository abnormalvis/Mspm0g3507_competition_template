#ifndef _MT_TEST_H
#define _MT_TEST_H

#include "stdint.h"

#define speed_expect_default         0.0f
#define speed_err_max                80.0f
#define speed_integral_max           60.0f
#define speed_ctrl_output_max        3000.0f
#define motor_max_default            (85-1)

#define steer_deadzone               50
#define turn_scale_default           0.06
#define turn_kp_default1             30.0f
#define turn_ki_default1             0.0f
#define turn_kd_default1             150

extern float turn_kp, turn_ki, turn_kd;
extern float cam_turn_kp, cam_turn_ki, cam_turn_kd;
extern uint16_t gray_cnt;
extern float v_target_l, v_target_r, p_target_l, p_target_r;
extern float turn_scale, speed_setup, turn_output, turn_ctrl_pwm, speed_adjust;
extern float left_pwm, right_pwm;
extern float yaw_target;
extern float turn_kp_L, turn_ki_L, turn_kd_L;
extern float yaw_kp, yaw_ki, yaw_kd;
extern float position_kp, position_kd;
extern float tar_theta_limit, sleep_time;

void motor_self_test(void);
void pid_params_init(void);

#endif
