#include "tracking_loop.h"
#include "StandardPid.h"
#include "imu_filter.h"   /* imu.yaw for the lap monitor */

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

/* ---- lost-line recovery tunables ---- */
#define LOST_DIR_LATCH    15    /* latch the deviation side once |position_error| exceeds this */
#define LOST_DEBOUNCE      2    /* start recovery pivot only after the line is lost this many ticks */
#define LOST_TURN_SPEED  260    /* in-place pivot speed target while recovering a lost line */

static int      s_last_dir   = -1;   /* +1 = last deviated right, -1 = left, 0 = unknown */
static uint16_t s_lost_ticks = 0;   /* consecutive ticks with no line */
static int      s_was_lost   = 0;   /* set while recovering, cleared on re-acquire */

/* Clear track PID integral + derivative history so a large error jump does not
   produce a derivative "kick". Called on the lost->found transition. */
static void track_pid_reset_history(void)
{
    track_pid.ErrorInt   = 0.0f;
    track_pid.Error0     = 0.0f;
    track_pid.Error1     = 0.0f;
    track_pid.LastActual = 0.0f;
    track_pid.CurrentOut = 0.0f;
}

void tracking_loop_init(void)
{
    tracking_result.position_error = 0;
    tracking_result.pid_correction = 0.0f;
    tracking_result.sensor_count   = 0;
    tracking_result.on_line        = 0;

    s_last_dir   = -1;
    s_lost_ticks = 0;
    s_was_lost   = 0;
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

    /* Step 3: position error �? weighted centroid */
    if (raw_sum > 0)
    {
        /* e = weighted_sum / raw_sum  (already integer arithmetic from integer weights)
         * normalize to [-100, 100]: multiply by 100 / WEIGHT_MAX */
        tracking_result.position_error = (int16_t)(
            (float)weighted_sum / (float)raw_sum * (100.0f / (float)WEIGHT_MAX)
        );
        tracking_result.on_line = 1;

        /* remember which side we last deviated to, for lost-line recovery */
        if (tracking_result.position_error >= LOST_DIR_LATCH)
            s_last_dir = 1;
        else if (tracking_result.position_error <= -LOST_DIR_LATCH)
            s_last_dir = -1;
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
    /* Lost-line recovery: if the line has been gone for a few ticks, pivot in
       place toward the side we last deviated to, to steer back and re-acquire it.
       This also covers right-angle corners, where the line escapes to one side
       and the car briefly loses it. */
    if (!tracking_result.on_line)
    {
        s_lost_ticks++;
        if (s_lost_ticks >= LOST_DEBOUNCE && s_last_dir != 0)
        {
            float corr = (float)LOST_TURN_SPEED * (float)s_last_dir;
            tracking_result.pid_correction = corr;
            *motor_left_out  =  corr;   /* dir>0 (last right) -> left fwd / right rev = turn right */
            *motor_right_out = -corr;
            s_was_lost = 1;
            return;
        }
        /* brief dropout: fall through and let the PID coast on the held error */
    }
    else
    {
        s_lost_ticks = 0;
    }

    /* On re-acquiring the line after a recovery pivot, clear PID history so the
       error jump doesn't cause a derivative kick. */
    if (s_was_lost && tracking_result.on_line)
    {
        track_pid_reset_history();
        s_was_lost = 0;
    }
    /* Step 4: PID computation �? target=0 (center line), actual=position_error */
    ComputePos(&track_pid, 0.0f, (float)tracking_result.position_error);
    tracking_result.pid_correction = track_pid.CurrentOut;

    /* Step 5: differential steering. Sign flipped vs the textbook law because
       the physical L/R wiring (motor and/or encoder side) is reversed, which
       made the un-flipped law steer the WRONG way -> the car diverged off the
       line instead of converging. Verified empirically (release-on-line test). */
    *motor_left_out  = base_speed + tracking_result.pid_correction;
    *motor_right_out = base_speed - tracking_result.pid_correction;
}

/* ================= lap monitor (IMU-yaw based) ================= */

static float s_yaw_prev  = 0.0f;   /* last raw imu.yaw sample */
static float s_yaw_cont  = 0.0f;   /* unwrapped continuous heading (deg) */
static float s_lap_base  = 0.0f;   /* heading at the start of the current lap */
static int   s_lap_count = 0;      /* completed laps since reset */
static int   s_lap_phase = 0;      /* ordered checkpoint 0->90->180->270->360 */

void lap_monitor_reset(void)
{
    s_yaw_prev  = imu.yaw;
    s_yaw_cont  = 0.0f;
    s_lap_base  = 0.0f;
    s_lap_count = 0;
    s_lap_phase = 0;
}

int lap_monitor_update(void)
{
    /* unwrap the incremental yaw (imu.yaw is wrapped to (-180,180]) into a
       continuous heading, so a full turn accumulates a real 360 deg */
    float d = imu.yaw - s_yaw_prev;
    if (d >  180.0f) d -= 360.0f;
    if (d < -180.0f) d += 360.0f;
    s_yaw_cont += d;
    s_yaw_prev  = imu.yaw;

    /* progress within the current lap, magnitude -> works for either turn dir */
    float mag = s_yaw_cont - s_lap_base;
    if (mag < 0.0f) mag = -mag;

    /* ordered checkpoints with 15 deg tolerance: jitter cannot jump straight to
       a lap, a genuine 0->90->180->270->360 sweep is required */
    if      (s_lap_phase == 0 && mag >=  75.0f) s_lap_phase = 1;
    else if (s_lap_phase == 1 && mag >= 165.0f) s_lap_phase = 2;
    else if (s_lap_phase == 2 && mag >= 255.0f) s_lap_phase = 3;
    else if (s_lap_phase == 3 && mag >= 345.0f)
    {
        s_lap_count++;
        s_lap_base  = s_yaw_cont;   /* next lap counts from here */
        s_lap_phase = 0;
    }

    return s_lap_count;
}
