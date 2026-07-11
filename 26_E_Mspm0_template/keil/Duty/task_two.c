#include "task_two.h"
#include "menu_task.h"
#include "tracking_loop.h"
#include "Encoder.h"
#include "StandardPid.h"
#include <stdlib.h>

/* ---- state chain: sequential hand-off flags ---- */
static int s_line_AC   = 1;
static int s_turn_CB   = 0;
static int s_line_BD   = 0;
static int s_line_BD_2 = 0;
static int s_line_BD_3 = 0;
static int s_turn_DA   = 0;
static int s_stop      = 0;
static int s_stop_2    = 0;

/* ---- encoder recording for arc start points ---- */
static int s_encL_bd = 0;
static int s_encR_bd = 0;

/* ---- arc parameters ---- */
static int s_arc_left_enc  = 3500;
static int s_arc_right_enc = 2500;
static int s_arc_left_spd  = 49;
static int s_arc_right_spd = 40;

/* ---- PIDs ---- */
static PidStruct s_pid_left;
static PidStruct s_pid_right;

void task_two_init(void)
{
    s_line_AC   = 1;
    s_turn_CB   = 0;
    s_line_BD   = 0;
    s_line_BD_2 = 0;
    s_line_BD_3 = 0;
    s_turn_DA   = 0;
    s_stop      = 0;
    s_stop_2    = 0;

    SetPidStruct(&s_pid_left,  0.5f, 0.0f, 0.0f, 0.0f, -3200.0f, 3200.0f);
    SetPidStruct(&s_pid_right, 0.5f, 0.0f, 0.0f, 0.0f, -3200.0f, 3200.0f);
}

void task_two_run(void)
{
    tracking_read();

    /* ---- intersection / probe trigger ---- */
    if (tracking_result.sensor_count >= GRAY_INTERSECTION_THRESHOLD)
    {
        if (s_line_AC)
        {
            s_turn_CB = 1;
            g_motor_left_out  = (float)(task_speed_base);
            g_motor_right_out = (float)(task_speed_base);
        }
        if (s_line_BD_2)
        {
            s_line_BD_2 = 0;
            s_turn_CB  = 0;
            s_encL_bd  = -Motor_distanceL;
            s_encR_bd  = Motor_distanceR;
        }

        /* BD first semi-arc */
        if (s_line_BD)
        {
            ComputePos(&s_pid_left,  (float)(s_encL_bd + 2500), (float)Motor_distanceL);
            ComputePos(&s_pid_right, (float)(s_encR_bd + 3100), (float)Motor_distanceR);
            g_motor_left_out  = s_pid_left.CurrentOut;
            g_motor_right_out = s_pid_right.CurrentOut;

            if (Motor_distanceR > (s_encR_bd + 3050))
            {
                s_line_BD   = 0;
                s_line_BD_3 = 1;
                s_encL_bd  += 3050;
                s_encR_bd  += 2500;
            }
        }

        /* BD second semi-arc */
        if (s_line_BD_3)
        {
            ComputePos(&s_pid_left,  (float)(s_encL_bd + s_arc_left_enc),  (float)Motor_distanceL);
            ComputePos(&s_pid_right, (float)(s_encR_bd + s_arc_right_enc), (float)Motor_distanceR);
            g_motor_left_out  = s_pid_left.CurrentOut;
            g_motor_right_out = s_pid_right.CurrentOut;
        }

        if (s_stop)
        {
            s_turn_DA = 0;
            s_stop    = 0;
            s_stop_2  = 1;
        }
        if (s_stop_2)
        {
            g_motor_left_out  = 0;
            g_motor_right_out = 0;
            task_running = 0;
            menu_active  = 1;
        }
    }
    else
    {
        /* normal tracking: follow line */
        if (s_line_AC)
        {
            s_turn_CB  = 1;
            s_line_AC  = 0;
            s_line_BD  = 1;
            s_line_BD_2 = 1;
        }
        if (s_line_BD_3)
        {
            s_line_BD_3 = 0;
            s_turn_DA   = 1;
            s_stop      = 1;
        }

        /* line following with fault correction */
        tracking_apply((float)task_speed_base, &g_motor_left_out, &g_motor_right_out);

        /* probe debounce: handled by s_turn_CB once per intersection */
        if (s_turn_CB)
        {
            s_turn_CB = 0;
        }
    }
}
