#include "task_one.h"
#include "menu_task.h"
#include "hal_gray.h"
#include "Encoder.h"
#include "StandardPid.h"
#include "Motor.h"
#include <stdlib.h>

/* ---- gray sensor fault table (8 sensors, center-weighted) ---- */
static const int fault_table[8] = {70, 50, 30, 10, -10, -30, -50, -70};

/* ---- task state ---- */
static int  s_turn_flag = 0;
static int  s_straight_count = 0;
static int  s_lap_count = 0;
static int  s_enc_target = 0;
static int  s_fault = 0;
static int  s_led_many = 0;

/* ---- position PID for clearing intersection ---- */
static PidStruct s_pos_pid;

void task_one_init(void)
{
    s_turn_flag = 0;
    s_straight_count = 0;
    s_lap_count = 0;
    s_enc_target = 0;
    s_fault = 0;

    SetPidStruct(&s_pos_pid, 0.5f, 0.0f, 0.0f, 0.0f, -3200.0f, 3200.0f);
}

static void task_tracking_read(void)
{
    uint8_t i;
    int32_t raw_sum = 0;
    int32_t weighted_sum = 0;

    s_led_many = 0;
    gray_8data_read();

    for (i = 0; i < 8; i++)
    {
        if (LQ_Tracking_Value[i] > gray_threshold[i])
        {
            s_led_many++;
            weighted_sum += fault_table[i];
            raw_sum++;
        }
    }

    if (raw_sum > 0)
        s_fault = weighted_sum / raw_sum;
    else
        s_fault = 0;
}

void task_one_run(void)
{
    task_tracking_read();

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
        g_motor_left_out = (float)(task_speed_base - s_fault);
        g_motor_right_out = (float)(task_speed_base + s_fault);
    }

    /* intersection detection: most sensors see black */
    if (s_led_many >= 6)
    {
        s_turn_flag = 1;
    }
}
