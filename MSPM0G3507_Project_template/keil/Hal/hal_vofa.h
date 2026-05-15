//
// Created by fazhehy on 2024/3/17.
//

#ifndef VOFA_H
#define VOFA_H

#include "stdint.h"

#define VOFA_FLOAT_NUM   10
#define VOFA_PARAM_MAX  16

/* VOFA parameter ID definitions */
/* Speed loop PID */
#define VOFA_PARAM_SPEED_KP      1
#define VOFA_PARAM_SPEED_KI     2
#define VOFA_PARAM_SPEED_KD     3
#define VOFA_PARAM_SPEED_TARGET 4
/* Angle loop PID */
#define VOFA_PARAM_ANGLE_KP     5
#define VOFA_PARAM_ANGLE_KI     6
#define VOFA_PARAM_ANGLE_KD     7
#define VOFA_PARAM_ANGLE_TARGET 8
/* Seektrack loop PID */
#define VOFA_PARAM_SEEK_KP      9
#define VOFA_PARAM_SEEK_KI      10
#define VOFA_PARAM_SEEK_KD      11
#define VOFA_PARAM_SEEK_TARGET   12
#define VOFA_PARAM_WHEEL_RADIUS  13

void vofa_add_data(float data);
void vofa_send(void);
void vofa_param_init(void);
uint8_t vofa_get_param_id(void);
uint8_t vofa_peek_param_id(void);
uint8_t vofa_has_param_update(void);
float vofa_get_param_value(uint8_t id);
void vofa_uart_rx_callback(uint8_t byte);

/* Send feedback data to VOFA for tuning */
void vofa_send_speed_feedback(float target_l, float actual_l, float actual_r, float kp, float ki);
void vofa_send_angle_feedback(float target, float actual);
void vofa_send_seek_feedback(float target, float actual);
void vofa_debug_enable(uint8_t enable);
void vofa_set_debug_mode(uint8_t mode);
uint8_t vofa_get_debug_mode(void);
uint8_t vofa_get_speed_pid(float *kp, float *ki);  /* returns bitmask: 1=KP, 2=KI, 3=both */
float vofa_get_speed_target(void);
float vofa_peek_speed_target(void);

#endif //VOFA_H