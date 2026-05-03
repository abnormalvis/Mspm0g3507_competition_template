//
// Created by fazhehy on 2024/3/17.
//

#ifndef VOFA_H
#define VOFA_H

#include "stdint.h"

#define VOFA_FLOAT_NUM   10

void vofa_add_data(float data);
void vofa_send(void);
void vofa_rx_byte(uint8_t byte);
void vofa_param_update(void);
uint8_t vofa_new_data(void);
float vofa_get_param(uint8_t id);

#endif //VOFA_H