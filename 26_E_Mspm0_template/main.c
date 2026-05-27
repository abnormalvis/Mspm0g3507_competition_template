#include "ti_msp_dl_config.h"
#include "interrupt_config.h"
#include "hal_gray.h"
#include "hal_imu.h"
#include "vofa.h"
#include "StandardPid.h"
#include "Encoder.h"
#include "Delay.h"
#include "zuolan_hmi.h"
#include "hmi_protocol.h"
#include "Serial.h"
#include "app.h"
#include "menu_task.h"
#include "task_manager.h"

volatile uint8_t gray_sample_req = 0;
volatile uint32_t sys_tick_ms = 0;

float vofa_speed_target = 0.0f; // speed target set via VOFA #P4=xxx!

static void on_vofa_param(uint16_t id, float value)
{
    switch (id)
    {
    case 1:
        MotorLSpeedPID.Kp = value;
        MotorRSpeedPID.Kp = -value;
        break;
    case 2:
        MotorLSpeedPID.Ki = value;
        MotorRSpeedPID.Ki = -value;
        break;
    case 3:
        MotorLSpeedPID.Kd = value;
        MotorRSpeedPID.Kd = -value;
        break;
    case 4:
        vofa_speed_target = value;
        break;
    case 5:
        MotorLSpeedPID.Kp = value;
        break;
    case 6:
        MotorLSpeedPID.Ki = value;
        break;
    case 7:
        MotorLSpeedPID.Kd = value;
        break;
    case 8:
        MotorRSpeedPID.Kp = value;
        break;
    case 9:
        MotorRSpeedPID.Ki = value;
        break;
    case 10:
        MotorRSpeedPID.Kd = value;
        break;
    case 11:
        yaw_pid.Kp = value;
        break;
    case 12:
        yaw_pid.Ki = value;
        break;
    case 13:
        yaw_pid.Kd = value;
        break;
    case 14:
        track_pid.Kp = value;
        break;
    case 15:
        track_pid.Ki = value;
        break;
    case 16:
        track_pid.Kd = value;
        break;
    default:
        break;
    }
}

int main(void)
{
    SYSCFG_DL_init();
    interrupt_init();

    // LCD_Init();
    //OLED_CLS();

    // hal_KeyInit();
    hal_imu_init();
    SysPidInit();
    EncoderFilterInit();

    vofa_init();
    vofa_set_on_param(on_vofa_param);

    AppInit();
    menu_init();
    task_manager_init();

    /* debug: verify code reaches this point via UART0 */
    uart_debug_send_byte('H');
    uart_debug_send_byte('M');
    uart_debug_send_byte('I');
    uart_debug_send_byte('\r');
    uart_debug_send_byte('\n');

    /* HMI serial screen startup message */
    zuolan_printf("page1.g0.txt=\"MSPM0 Ready\"%s", HMI_END_CMD);

    while (1)
    {
        SensorProc();
        float pid_ch[5] = {
            vofa_speed_target,
            (float)Motor_speedL,
            // (float)Motor_speedR,
            MotorLSpeedPID.Kp,
            MotorLSpeedPID.Ki,
            MotorLSpeedPID.Kd};
        vofa_send_floats(pid_ch, 5);

        /* ---- HMI serial screen event polling ---- */
        if (hmi_rx_ready)
        {
            hmi_rx_ready = 0;
            hmi_event_t evt;
            if (hmi_parse_frame(hmi_rx_buf, hmi_rx_idx - 1, &evt) == 0)
            {
                hmi_dispatch_event(&evt);
            }
            hmi_rx_idx = 0;
        }

        /* ---- HMI telemetry: every 200ms send debug data to screen ---- */
        static uint32_t last_hmi_telem = 0;
        if (sys_tick_ms - last_hmi_telem >= 200)
        {
            last_hmi_telem = sys_tick_ms;
            zuolan_HMI_Send_Int("debug.speed_l", Motor_speedL);
            zuolan_HMI_Send_Int("debug.speed_r", Motor_speedR);
            zuolan_HMI_Send_Float("debug.kp", MotorLSpeedPID.Kp, 2);
            zuolan_HMI_Send_Float("debug.ki", MotorLSpeedPID.Ki, 2);
        }

        /* ---- HMI task completion: restore button text ---- */
        static uint8_t was_task_running = 0;
        if (!task_running && was_task_running)
        {
            zuolan_printf("b0.txt=\"题目1\"%s", HMI_END_CMD);
            zuolan_printf("b1.txt=\"题目2\"%s", HMI_END_CMD);
            zuolan_printf("b2.txt=\"题目3\"%s", HMI_END_CMD);
            zuolan_printf("b3.txt=\"题目4\"%s", HMI_END_CMD);
        }
        was_task_running = task_running;

        if (g_stop_requested)
        {
            g_stop_requested = 0;
            task_running = 0;
            menu_active = 1;
            g_motor_left_out = 0;
            g_motor_right_out = 0;
            //OLED_CLS();
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
        if ((sys_tick_ms % 5) == 0)
            hal_imu_update();
        if ((sys_tick_ms % 5) == 0)
            gray_sample_req = 1;
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
            static float target_last = 0.0f;
            float abs_target, abs_L, abs_R;

            abs_target = (vofa_speed_target >= 0.0f) ? vofa_speed_target : -vofa_speed_target;
            abs_L     = (Motor_speedL >= 0) ? (float)Motor_speedL : -(float)Motor_speedL;
            abs_R     = (Motor_speedR >= 0) ? (float)Motor_speedR : -(float)Motor_speedR;

            /* reset integrator & output on direction reversal */
            if ((target_last >= 0.0f) != (vofa_speed_target >= 0.0f))
            {
                MotorLSpeedPID.ErrorInt = 0.0f;
                MotorRSpeedPID.ErrorInt = 0.0f;
                MotorLSpeedPID.CurrentOut = 0.0f;
                MotorRSpeedPID.CurrentOut = 0.0f;
            }
            target_last = vofa_speed_target;

            ComputeInc(&MotorLSpeedPID, abs_target, abs_L);
            ComputeInc(&MotorRSpeedPID, abs_target, abs_R);

            g_motor_left_out  = (vofa_speed_target >= 0.0f)
                              ? MotorLSpeedPID.CurrentOut
                              : -MotorLSpeedPID.CurrentOut;
            g_motor_right_out = (vofa_speed_target >= 0.0f)
                              ? MotorRSpeedPID.CurrentOut
                              : -MotorRSpeedPID.CurrentOut;
        }

        Motor_SetPWML(g_motor_left_out);
        Motor_SetPWMR(g_motor_right_out);
        break;
    default:
        break;
    }
}

/**
 * GROUP1 = GPIOB interrupt aggregation: key falling edge + encoder A-phase edge
 */
void GROUP1_IRQHandler(void)
{
    uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIOB,
                                                       Encoder_Encoder1_A_PIN);

    if (gpioB & Encoder_Encoder1_A_PIN)
    {
        Encoder_OnGroupIRQ(gpioB);
    }
}
