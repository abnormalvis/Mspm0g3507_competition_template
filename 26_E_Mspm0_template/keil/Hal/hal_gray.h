#ifndef __HAL_GRAY_H
#define __HAL_GRAY_H

#include <stdint.h>

extern uint16_t LQ_Tracking_Value[8];
extern uint16_t gray_threshold[8];

typedef struct
{
    uint8_t bit1  : 1;
    uint8_t bit2  : 1;
    uint8_t bit3  : 1;
    uint8_t bit4  : 1;
    uint8_t bit5  : 1;
    uint8_t bit6  : 1;
    uint8_t bit7  : 1;
    uint8_t bit8  : 1;
    uint8_t bit9  : 1;
    uint8_t bit10 : 1;
    uint8_t bit11 : 1;
    uint8_t bit12 : 1;
} gray_flags;

typedef union
{
    gray_flags gray;
    uint16_t   state;
} _gray_state;

extern _gray_state gray_state;
extern float       gray_status;
extern char        stop_flag;

void    gray_8data_read(void);
void    gray_set_threshold(uint16_t *threshold);
uint8_t gray_is_worse(void);

#endif
