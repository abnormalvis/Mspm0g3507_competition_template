#include "task_four.h"
#include "menu_task.h"
#include "hal_gray.h"
#include "Encoder.h"
#include "StandardPid.h"
#include <stdlib.h>

/* ---- gray sensor fault table ---- */
static const int fault_table[8] = {70, 50, 30, 10, -10, -30, -50, -70};

/* ---- state chain ---- */
static int s_line_AC    = 1;
static int s_line_AC_f  = 0;
static int s_line_AC_2  = 0;
static int s_line_AC_3  = 0;
static int s_turn_CB    = 0;
static int s_turn_CB_2  = 0;
static int s_line_BD    = 0;
static int s_line_BD_2  = 0;
static int s_line_BD_3  = 0;
static int s_turn_DA    = 0;
static int s_turn_DA_2  = 0;
static int s_stop       = 0;
static int s_stop_2     = 0;
static int s_stop_last  = 0;
static int s_lap_flag   = 1;

/* ---- encoder recording ---- */
static int s_encL_bd = 0;
static int s_encR_bd = 0;
static int s_encL_ac = 0;
static int s_encR_ac = 0;

/* ---- tracking ---- */
static int s_fault    = 0;
static int s_led_many = 0;

/* ---- arc parameters ---- */
static int s_arcL_enc1 = 3500, s_arcR_enc1 = 2500;
static int s_arcL_enc2 = 2500, s_arcR_enc2 = 3500;

/* ---- probe debounce ---- */
static int s_probe_debounce = 0;
static int s_probe_timer    = 0;

/* ---- sound/light ---- */
static int s_sg_active = 0;
static int s_sg_timer  = 0;

/* ---- PIDs ---- */
static PidStruct s_pid_left;
static PidStruct s_pid_right;

void task_four_init(void)
{
    s_line_AC    = 1;
    s_line_AC_f  = 0;
    s_line_AC_2  = 0;
    s_line_AC_3  = 0;
    s_turn_CB    = 0;
    s_turn_CB_2  = 0;
    s_line_BD    = 0;
    s_line_BD_2  = 0;
    s_line_BD_3  = 0;
    s_turn_DA    = 0;
    s_turn_DA_2  = 0;
    s_stop       = 0;
    s_stop_2     = 0;
    s_stop_last  = 0;
    s_lap_flag   = 1;
    s_fault      = 0;
    s_probe_debounce = 0;
    s_probe_timer    = 0;
    s_sg_active = 0;
    s_sg_timer  = 0;

    SetPidStruct(&s_pid_left,  0.5f, 0.0f, 0.0f, 0.0f, -3200.0f, 3200.0f);
    SetPidStruct(&s_pid_right, 0.5f, 0.0f, 0.0f, 0.0f, -3200.0f, 3200.0f);
}

static void task_tracking_read(void)
{
    uint8_t i;
    int32_t weighted_sum = 0;
    int32_t raw_sum = 0;

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
    s_fault = (raw_sum > 0) ? (weighted_sum / raw_sum) : 0;
}

void task_four_run(void)
{
    task_tracking_read();

    /* ---- sound/light timer ---- */
    if (s_sg_active && s_sg_timer > 0)
    {
        s_sg_timer--;
        if (s_sg_timer == 0)
            s_sg_active = 0;
    }

    /* ---- probe debounce timer ---- */
    if (s_probe_debounce && s_probe_timer > 0)
    {
        s_probe_timer--;
        if (s_probe_timer == 0)
            s_probe_debounce = 0;
    }

    /* ---- intersection detect ---- */
    if ((s_led_many >= 6) && !s_probe_debounce)
    {
        /* lap 1: same as task_three */
        if (s_line_AC && (s_lap_flag == 1))
        {
            s_turn_CB = 1;
            g_motor_left_out  = (float)(task_speed_base);
            g_motor_right_out = (float)(task_speed_base);
        }

        /* lap 2+: reverse direction on AC */
        if (s_line_AC && (s_lap_flag > 1))
        {
            if (s_line_AC_2)
            {
                s_line_AC_2 = 0;
                s_line_AC_f = 1;
                s_encL_ac = -Motor_distanceL;
                s_encR_ac = Motor_distanceR;
            }

            if (s_line_AC_f)
            {
                ComputePos(&s_pid_left,  (float)(s_encL_ac + 3100), (float)Motor_distanceL);
                ComputePos(&s_pid_right, (float)(s_encR_ac + 2500), (float)Motor_distanceR);
                g_motor_left_out  = s_pid_left.CurrentOut;
                g_motor_right_out = s_pid_right.CurrentOut;

                if (Motor_distanceR > (s_encR_ac + 2350))
                {
                    s_line_AC_f = 0;
                    s_line_AC_3 = 1;
                    s_encL_ac  += 2500;
                    s_encR_ac  += 3050;
                }
            }

            if (s_line_AC_3)
            {
                ComputePos(&s_pid_left,  (float)(s_encL_ac + s_arcL_enc2), (float)Motor_distanceL);
                ComputePos(&s_pid_right, (float)(s_encR_ac + s_arcR_enc2), (float)Motor_distanceR);
                g_motor_left_out  = s_pid_left.CurrentOut;
                g_motor_right_out = s_pid_right.CurrentOut;
            }
        }

        if (s_line_BD_2 && !s_sg_active)
        {
            s_sg_active = 1;
            s_sg_timer  = 50;
            s_line_BD_2 = 0;
            s_turn_CB   = 0;
            s_encL_bd   = -Motor_distanceL;
            s_encR_bd   = Motor_distanceR;
        }

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

        if (s_line_BD_3)
        {
            ComputePos(&s_pid_left,  (float)(s_encL_bd + s_arcL_enc1), (float)Motor_distanceL);
            ComputePos(&s_pid_right, (float)(s_encR_bd + s_arcR_enc1), (float)Motor_distanceR);
            g_motor_left_out  = s_pid_left.CurrentOut;
            g_motor_right_out = s_pid_right.CurrentOut;
        }

        if (s_stop && !s_sg_active)
        {
            s_sg_active = 1;
            s_sg_timer  = 50;
            s_turn_DA   = 0;
            s_stop      = 0;
            s_stop_2    = 1;
        }
        if (s_stop_2)
        {
            s_lap_flag++;
            if (s_lap_flag > task4_max_laps)
            {
                s_stop_last = 1;
            }
            s_line_AC   = 1;
            s_line_AC_2 = 1;
            s_stop_2    = 0;
        }
        if (s_stop_last)
        {
            g_motor_left_out  = 0;
            g_motor_right_out = 0;
            s_line_AC   = 0;
            s_line_AC_2 = 0;
            task_running = 0;
            menu_active  = 1;
        }
    }
    else
    {
        if (s_line_AC && !s_sg_active)
        {
            s_sg_active = 1;
            s_sg_timer  = 50;
            s_turn_CB   = 1;
            s_line_AC   = 0;
            s_line_BD   = 1;
            s_line_BD_2 = 1;
        }
        if (s_line_BD_3 && !s_sg_active)
        {
            s_sg_active = 1;
            s_sg_timer  = 50;
            s_line_BD_3 = 0;
            s_turn_DA   = 1;
            s_stop      = 1;
        }

        /* probe debounce after first intersection */
        if (s_turn_CB)
        {
            s_probe_debounce = 1;
            s_probe_timer    = 150;
            s_line_AC   = 0;
            s_line_AC_3 = 0;
            s_turn_CB   = 0;
            s_turn_CB_2 = 1;
        }

        if (s_turn_CB_2)
        {
            g_motor_left_out  = (float)(task_speed_base - s_fault);
            g_motor_right_out = (float)(task_speed_base + s_fault);
        }
        if (s_turn_DA_2)
        {
            s_line_AC_2 = 1;
            g_motor_left_out  = (float)(task_speed_base - s_fault);
            g_motor_right_out = (float)(task_speed_base + s_fault);
        }
    }
}
