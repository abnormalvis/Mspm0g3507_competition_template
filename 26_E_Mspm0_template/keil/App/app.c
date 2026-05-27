/*******************************************************************************
 * @brief  Telemetry display â€? runs when no menu/task is active
 ********************************************************************************/
#include "ti_msp_dl_config.h"
#include "app.h"
#include "lcd.h"
#include "hal_key.h"
#include "hal_gray.h"
#include "hal_imu.h"
#include "vofa.h"
#include "StandardPid.h"
#include "Encoder.h"
#include "imu_filter.h"
#include <stdio.h>

#ifndef pi
#define pi 3.14159265358979f
#endif

/* ---- local state flags ---- */
typedef struct {
    unsigned char Start_Car;
    unsigned char Start_duty_1;
    unsigned char task_start;
    unsigned char beep_on;
} AppFlag;
typedef struct {
    unsigned char Send_Step;
    unsigned char Send2_Step;
} AppParam;
typedef struct {
    float left_motor_speed_cmps;
    float right_motor_speed_cmps;
} AppSmartcarImu;

static AppFlag Flag;
static AppParam Param;
static AppSmartcarImu smartcar_imu;

static float point_A[2], point_B[2], point_C[2], point_D[2], point_actual[2];
static float pos_out_limH = 0.0f, pos_out_limL = 0.0f, speed_adjust = 1.0f;
static float tar_theta_limit = 0.0f, pos_boudary = 0.0f, turn_kp = 0.0f;
static float v_target_l = 0.0f, v_target_r = 0.0f, speed_setup = 0.0f;
static float distance_inter = 0.0f;

extern volatile uint8_t gray_sample_req;

static uint16_t gray_vofa_send_cnt = 0;

static void Flag_Init(void)
{
    Flag.task_start = 0;
    Flag.beep_on = 0;
    Flag.Start_duty_1 = 0;
    Flag.Start_Car = 0;
}

void AppInit(void)
{
    uint8_t i;

    /* default gray thresholds */
    for (i = 0; i < 8; i++)
        gray_threshold[i] = 2000;

    Flag_Init();
    gray_vofa_send_cnt = 0;
}

/* ---- simple desktop display, cycling rows each call ---- */
static void telemetry_display(void)
{
    static uint8_t display_state = 1;

    switch (display_state)
    {
    case 1:
        display_6_8_string(0, 1, "Task Running...");
        break;
    case 2:
        LCD_clear_L(0, 2);
        display_6_8_string(0, 2, "L_speed:");
        display_6_8_number(48, 2, (float)Motor_speedL);
        display_6_8_string(0, 3, "R_speed:");
        display_6_8_number(48, 3, (float)Motor_speedR);
        break;
    case 3:
        LCD_clear_L(0, 4);
        display_6_8_string(0, 4, "yaw:");
        display_6_8_number(30, 4, imu.yaw);
        break;
    case 4:
        LCD_clear_L(0, 5);
        display_6_8_string(0, 5, "distL:");
        display_6_8_number(36, 5, (float)Motor_distanceL);
        display_6_8_string(0, 6, "distR:");
        display_6_8_number(36, 6, (float)Motor_distanceR);
        break;
    default:
        display_state = 0;
        break;
    }
    display_state++;
    if (display_state > 4) display_state = 1;
}

void SensorProc(void)
{
    if (gray_sample_req)
    {
        uint8_t i;
        gray_sample_req = 0;

        gray_8data_read();

        gray_state.gray.bit1 = (LQ_Tracking_Value[0] > gray_threshold[0]);
        gray_state.gray.bit2 = (LQ_Tracking_Value[1] > gray_threshold[1]);
        gray_state.gray.bit3 = (LQ_Tracking_Value[2] > gray_threshold[2]);
        gray_state.gray.bit4 = (LQ_Tracking_Value[3] > gray_threshold[3]);
        gray_state.gray.bit5 = (LQ_Tracking_Value[4] > gray_threshold[4]);
        gray_state.gray.bit6 = (LQ_Tracking_Value[5] > gray_threshold[5]);
        gray_state.gray.bit7 = (LQ_Tracking_Value[6] > gray_threshold[6]);
        gray_state.gray.bit8 = (LQ_Tracking_Value[7] > gray_threshold[7]);

        gray_status = 0;
        for (i = 0; i < 8; i++)
        {
            if (LQ_Tracking_Value[i] > gray_threshold[i])
                gray_status += (1 << i);
        }

        gray_vofa_send_cnt++;
        if (gray_vofa_send_cnt >= 10)
        {
            gray_vofa_send_cnt = 0;
            {
                float ch[9] = {
                    gray_status,
                    (float)LQ_Tracking_Value[0], (float)LQ_Tracking_Value[1],
                    (float)LQ_Tracking_Value[2], (float)LQ_Tracking_Value[3],
                    (float)LQ_Tracking_Value[4], (float)LQ_Tracking_Value[5],
                    (float)LQ_Tracking_Value[6], (float)LQ_Tracking_Value[7]};
                vofa_send_floats(ch, 9);
            }
        }
    }
}

void AppProc(void)
{
    telemetry_display();
}
