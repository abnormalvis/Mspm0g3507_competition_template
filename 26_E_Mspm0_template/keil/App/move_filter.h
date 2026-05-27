#ifndef _MOVE_FILTER_H
#define _MOVE_FILTER_H

#define MOVE_AVERAGE_SIZE       5
#define SPEED_LOWPASS_ALPHA     0.05f

#include "stdint.h"

typedef struct
{
    uint8_t index;
    uint8_t buffer_size;
    float data_buffer[MOVE_AVERAGE_SIZE];
    float data_sum;
    float data_average;
} move_filter_struct;

void move_filter_init(move_filter_struct *move_average);
void move_filter_calc(move_filter_struct *move_average, float new_data);

typedef struct
{
    float alpha;
    float output;
} lowpass_filter_struct;

void lowpass_filter_init(lowpass_filter_struct *f, float alpha);
float lowpass_filter_calc(lowpass_filter_struct *f, float input);
void lowpass_filter_set_alpha(lowpass_filter_struct *f, float alpha);

#endif
