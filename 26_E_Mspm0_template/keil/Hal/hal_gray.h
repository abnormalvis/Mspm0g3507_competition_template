#ifndef __HAL_GRAY_H
#define __HAL_GRAY_H

#include <stdint.h>

/* ---- Gray sensor type selection ---- */
// #define GRAY_SENSOR_16CH   // uncomment for 16ch; keep commented for 8ch (default)

#ifdef GRAY_SENSOR_16CH
#define GRAY_CHANNEL_COUNT          16
#define GRAY_INTERSECTION_THRESHOLD 12
#else
#define GRAY_CHANNEL_COUNT          8
#define GRAY_INTERSECTION_THRESHOLD 6
#endif

extern uint16_t LQ_Tracking_Value[GRAY_CHANNEL_COUNT];
extern uint16_t gray_threshold[GRAY_CHANNEL_COUNT];

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
    uint8_t bit13 : 1;
    uint8_t bit14 : 1;
    uint8_t bit15 : 1;
    uint8_t bit16 : 1;
} gray_flags;

typedef union
{
    gray_flags gray;
    uint16_t   state;
} _gray_state;

extern _gray_state gray_state;
extern float       gray_status;
extern char        stop_flag;

/* ---- uni ed API ---- */
void    gray_init(void);
void    gray_read(void);
void    gray_8data_read(void);     /* backward-compat: calls gray_read() */
void    gray_set_threshold(uint16_t *threshold);
uint8_t gray_is_worse(void);

#endif
