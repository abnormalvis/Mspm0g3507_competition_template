//
// Created by fazhehy on 2024/3/17.
//

#ifndef VOFA_H
#define VOFA_H

#include "stdint.h"

#define VOFA_PARAM_MAX  20

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
#define VOFA_PARAM_SPEED_FILTER  14   /* speed loop low-pass filter alpha (0~1] */
/* Position loop PID */
#define VOFA_PARAM_POSITION_KP   15
#define VOFA_PARAM_POSITION_KI   16
#define VOFA_PARAM_POSITION_KD   17
#define VOFA_PARAM_POSITION_TARGET 18
/* VOFA runtime control */
#define VOFA_PARAM_MODE_FLAGS    19
#define VOFA_PARAM_TUNE_TYPE     20

typedef void (*vofa_param_callback_t)(uint16_t id, float value, void *user);

void vofa_param_init(void);
void vofa_register_param_callback(vofa_param_callback_t callback, void *user);
uint8_t vofa_get_param_id(void);
uint8_t vofa_peek_param_id(void);
uint8_t vofa_has_param_update(void);
float vofa_get_param_value(uint8_t id);
void vofa_uart_rx_callback(uint8_t byte);

#endif //VOFA_H