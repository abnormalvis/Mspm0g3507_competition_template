#include "ti_msp_dl_config.h"
#include "interrupt_config.h"
#include "lcd.h"
#include "lcd_init.h"
#include "hal_key.h"
#include "hal_gray.h"
#include "hal_imu.h"
#include "vofa.h"
#include "StandardPid.h"
#include "Encoder.h"
#include "Delay.h"
#include "app.h"
#include "menu_task.h"
#include "task_manager.h"

volatile uint8_t  gray_sample_req   = 0;
volatile uint32_t sys_tick_ms       = 0;

float vofa_speed_target = 0.0f;  // speed target set via VOFA #P4=xxx!

static void on_vofa_param(uint16_t id, float value)
{
    switch (id)
    {
        case  1: MotorLSpeedPID.Kp  = value;
                 MotorRSpeedPID.Kp  = -value;  break;
        case  2: MotorLSpeedPID.Ki  = value;
                 MotorRSpeedPID.Ki  = -value;  break;
        case  3: MotorLSpeedPID.Kd  = value;
                 MotorRSpeedPID.Kd  = -value;  break;
        case  4: vofa_speed_target  = value;  break;
        case  5: MotorLSpeedPID.Kp  = value;  break;
        case  6: MotorLSpeedPID.Ki  = value;  break;
        case  7: MotorLSpeedPID.Kd  = value;  break;
        case  8: MotorRSpeedPID.Kp  = value;  break;
        case  9: MotorRSpeedPID.Ki  = value;  break;
        case 10: MotorRSpeedPID.Kd  = value;  break;
        case 11: yaw_pid.Kp         = value;  break;
        case 12: yaw_pid.Ki         = value;  break;
        case 13: yaw_pid.Kd         = value;  break;
        case 14: track_pid.Kp       = value;  break;
        case 15: track_pid.Ki       = value;  break;
        case 16: track_pid.Kd       = value;  break;
        default: break;
    }
}

int main(void)
{
    SYSCFG_DL_init();
    interrupt_init();

    LCD_Init();
    OLED_CLS();

    hal_KeyInit();
    hal_imu_init();
    SysPidInit();

    vofa_init();
    vofa_set_on_param(on_vofa_param);

    AppInit();
    menu_init();
    task_manager_init();

    while (1)
    {
        if (g_stop_requested)
        {
            g_stop_requested = 0;
            task_running = 0;
            menu_active = 1;
            g_motor_left_out = 0;
            g_motor_right_out = 0;
            OLED_CLS();
        }

        if (menu_active)
        {
            menu_key_set();
        }
        else
        {
            AppProc();
        }
    }
}

/**
 * TIMER_0 (TIMG0) 1ms tick: key proc, IMU, gray sample request
 */
void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST))
    {
        case DL_TIMER_IIDX_ZERO:
            sys_tick_ms++;
            if ((sys_tick_ms %  5) == 0) hal_imu_update();
            if ((sys_tick_ms % 10) == 0) hal_KeyProc();
            if ((sys_tick_ms %  5) == 0) gray_sample_req = 1;
            break;
        default:
            break;
    }
}

/**
 * TIMER_1 (TIMA1) 10ms: encoder read -> speed PID -> motor PWM output
 */
void TIMER_1_INST_IRQHandler(void)
{
    switch (DL_TimerA_getPendingInterrupt(TIMER_1_INST))
    {
        case DL_TIMERA_IIDX_LOAD:
            EncoderGetValue();

            if (task_running)
            {
                task_manager_run();
            }
            else
            {
                ComputePos(&MotorLSpeedPID, vofa_speed_target, (float)Motor_speedL);
                ComputePos(&MotorRSpeedPID, vofa_speed_target, (float)Motor_speedR);
                g_motor_left_out  = MotorLSpeedPID.CurrentOut;
                g_motor_right_out = MotorRSpeedPID.CurrentOut;
            }

            Motor_SetPWML(g_motor_left_out);
            Motor_SetPWMR(g_motor_right_out);
            break;
        default:
            break;
    }
}

/**
 * GROUP1 = GPIOB �????�????聚合：键盘下降沿 + Encoder1 A 相边�????
 */
void GROUP1_IRQHandler(void)
{
    uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIOB,
        KEY_KEY_UP_PIN  | KEY_KEY_DOWN_PIN | KEY_KEY_LEFT_PIN |
        KEY_KEY_RIGHT_PIN | KEY_KEY_MID_PIN |
        Encoder_Encoder1_A_PIN | Encoder_Encoder2_A_PIN);

    if (gpioB & (KEY_KEY_UP_PIN | KEY_KEY_DOWN_PIN | KEY_KEY_LEFT_PIN |
                 KEY_KEY_RIGHT_PIN | KEY_KEY_MID_PIN))
    {
        hal_key_isr_notify(gpioB);
        DL_GPIO_clearInterruptStatus(KEY_PORT,
            gpioB & (KEY_KEY_UP_PIN | KEY_KEY_DOWN_PIN | KEY_KEY_LEFT_PIN |
                     KEY_KEY_RIGHT_PIN | KEY_KEY_MID_PIN));
    }

    if (gpioB & (Encoder_Encoder1_A_PIN | Encoder_Encoder2_A_PIN))
    {
        Encoder_OnGroupIRQ(gpioB);
    }
}
