//
// Created by fazhehy on 2024/3/17.
//

#ifndef VOFA_H
#define VOFA_H

#include "stdint.h"

#define VOFA_FLOAT_NUM   10
#define VOFA_PARAM_MAX  16

/* VOFA parameter ID definitions */
#define VOFA_PARAM_KP      1
#define VOFA_PARAM_KI     2
#define VOFA_PARAM_KD     3
#define VOFA_PARAM_TARGET 4

void vofa_add_data(float data);
void vofa_send(void);
void vofa_param_init(void);
uint8_t vofa_get_param_id(void);
float vofa_get_param_value(uint8_t id);
void vofa_uart_rx_callback(uint8_t byte);

#endif //VOFA_H