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

/* ---- lap monitor (IMU-yaw based) ----
 * Counts full 360-degree loops of the car by unwrapping imu.yaw (which is
 * wrapped to (-180,180]) into a continuous heading. One lap = yaw sweeps a
 * full turn, e.g. 0 -> -90 -> -180 -> -270 -> 0.  Direction-agnostic. */
void lap_monitor_reset(void);      /* call at task start (baseline = current yaw) */
int  lap_monitor_update(void);     /* call once per control tick; returns completed laps */

#endif
