/*******************************************************************************
 * @brief  Telemetry display �? runs when no menu/task is active
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
    for (i = 0; i < GRAY_CHANNEL_COUNT; i++)
        gray_threshold[i] = 2000;

    Flag_Init();
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
        gray_sample_req = 0;

        gray_read();

        /* gray sensor data now shown on HMI page2.tk_string; VOFA channel reserved for PID tuning */
    }
}

void AppProc(void)
{
    telemetry_display();
}
