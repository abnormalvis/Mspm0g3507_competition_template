#include "task_one.h"
#include "menu_task.h"
#include "tracking_loop.h"
#include "Encoder.h"
#include "StandardPid.h"
#include "Motor.h"
#include <stdlib.h>

/* ---- task state ---- */
static int  s_turn_flag = 0;
static int  s_straight_count = 0;
static int  s_lap_count = 0;
static int  s_enc_target = 0;

/* ---- position PID for clearing intersection ---- */
static PidStruct s_pos_pid;

void task_one_init(void)
{
    s_turn_flag = 0;
    s_straight_count = 0;
    s_lap_count = 0;
    s_enc_target = 0;

    SetPidStruct(&s_pos_pid, 0.5f, 0.0f, 0.0f, 0.0f, -3200.0f, 3200.0f);
}

void task_one_run(void)
{
    tracking_read();

    /* ---- lap completed check ---- */
    if ((s_lap_count >= task1_max_laps) && (s_straight_count == 1))
    {
        s_turn_flag = -1;
        g_motor_left_out = 0;
        g_motor_right_out = 0;
        return;
    }

    if (s_turn_flag == 1)
    {
        /* clearing intersection: left motor stop, right motor position PID */
        g_motor_left_out = 0;
        ComputePos(&s_pos_pid, (float)s_enc_target, (float)Motor_distanceR);

        if (abs(s_enc_target - Motor_distanceR) < 30)
        {
            s_enc_target = Motor_distanceR;
            s_turn_flag = 0;
            s_straight_count++;

            if (s_straight_count == 4)
            {
                s_straight_count = 0;
                s_lap_count++;
            }
        }
    }
    else if (s_turn_flag == 0)
    {
        /* normal line tracking: differential steering */
        s_enc_target = Motor_distanceR + 700;
        tracking_apply((float)task_speed_base, &g_motor_left_out, &g_motor_right_out);
    }

    /* intersection detection: most sensors see black */
    if (tracking_result.sensor_count >= GRAY_INTERSECTION_THRESHOLD)
    {
        s_turn_flag = 1;
    }
}
