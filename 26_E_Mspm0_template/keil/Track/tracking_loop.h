#ifndef __TRACKING_LOOP_H__
#define __TRACKING_LOOP_H__

#include "hal_gray.h"
#include <stdint.h>

/* ---- tracking result struct ---- */
typedef struct {
    int16_t position_error;    /* weighted centroid deviation from center */
    float   pid_correction;    /* PID output correction value */
    uint8_t sensor_count;      /* how many sensors see black line */
    uint8_t on_line;           /* 1 = line detected, 0 = lost line */
} TrackingResult;

extern TrackingResult tracking_result;

void tracking_loop_init(void);
void tracking_read(void);
void tracking_apply(float base_speed, float *motor_left_out, float *motor_right_out);

#endif
