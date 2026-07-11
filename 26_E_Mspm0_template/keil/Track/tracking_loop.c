#include "tracking_loop.h"
#include "StandardPid.h"

TrackingResult tracking_result;

#ifdef GRAY_SENSOR_16CH
static const int16_t position_weight[16] = {
    -15, -13, -11, -9, -7, -5, -3, -1,
      1,   3,   5,  7,  9, 11, 13, 15
};
#define WEIGHT_MAX  15
#else
static const int16_t position_weight[8] = {
    -7, -5, -3, -1, 1, 3, 5, 7
};
#define WEIGHT_MAX  7
#endif

void tracking_loop_init(void)
{
    tracking_result.position_error = 0;
    tracking_result.pid_correction = 0.0f;
    tracking_result.sensor_count   = 0;
    tracking_result.on_line        = 0;
}

/* ---- Step 1-3: read sensors, compute weighted centroid, normalize to [-100, 100] ---- */
void tracking_read(void)
{
    uint8_t i;
    int32_t weighted_sum = 0;
    int32_t raw_sum      = 0;

    /* Step 1: read all gray sensors */
    gray_read();

    /* Step 2: weighted centroid e = Σ(wi * si) / Σ(si)
     *         si = (value < threshold) ? 1 : 0  (black line = 1) */
    tracking_result.sensor_count = 0;
    for (i = 0; i < GRAY_CHANNEL_COUNT; i++)
    {
        if (LQ_Tracking_Value[i] < gray_threshold[i])
        {
            weighted_sum += position_weight[i];
            raw_sum++;
        }
    }
    tracking_result.sensor_count = (uint8_t)raw_sum;

    /* Step 3: position error — weighted centroid */
    if (raw_sum > 0)
    {
        /* e = weighted_sum / raw_sum  (already integer arithmetic from integer weights)
         * normalize to [-100, 100]: multiply by 100 / WEIGHT_MAX */
        tracking_result.position_error = (int16_t)(
            (float)weighted_sum / (float)raw_sum * (100.0f / (float)WEIGHT_MAX)
        );
        tracking_result.on_line = 1;
    }
    else
    {
        /* lost-line: hold last known position_error, keep on_line=0 */
        tracking_result.on_line = 0;
    }
}

/* ---- Step 4-5: PID correction -> differential motor output ---- */
void tracking_apply(float base_speed, float *motor_left_out, float *motor_right_out)
{
    /* Step 4: PID computation — target=0 (center line), actual=position_error */
    ComputePos(&track_pid, 0.0f, (float)tracking_result.position_error);
    tracking_result.pid_correction = track_pid.CurrentOut;

    /* Step 5: differential steering */
    *motor_left_out  = base_speed - tracking_result.pid_correction;
    *motor_right_out = base_speed + tracking_result.pid_correction;
}
