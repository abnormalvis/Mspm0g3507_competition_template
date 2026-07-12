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

void task_one_init(void)
{
    s_turn_flag = 0;
    s_straight_count = 0;
    s_lap_count = 0;
    s_enc_target = 0;
    lap_monitor_reset();   /* start counting laps from the current heading */
}

void task_one_run(void)
{
    /* Q1: start tracking automatically, stop after task1_max_laps full loops.
       One lap = imu.yaw sweeps a full turn (0 -> -90 -> -180 -> -270 -> 0). */
    tracking_read();

    if (lap_monitor_update() >= task1_max_laps)
    {
        g_motor_left_out  = 0;
        g_motor_right_out = 0;
        task_running = 0;
        menu_active  = 1;
        return;
    }

    tracking_apply((float)task_speed_base, &g_motor_left_out, &g_motor_right_out);
}

#if 0  /* 完整比赛逻辑：路口识别 + 圈数统计 + 跑满自动停车，调完PID后恢复 */
void task_one_run_full(void)
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
        ComputePos(&track_pid, (float)s_enc_target, (float)Motor_distanceR);

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
#endif
