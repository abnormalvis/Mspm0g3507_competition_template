#include "ti_msp_dl_config.h"
#include "interrupt_config.h"
#include "hal_gray.h"
#include "hal_imu.h"
#include "zf_device_imu660ra.h"
#include "imu_filter.h"
#include "vofa.h"
#include "StandardPid.h"
#include "Encoder.h"
#include "Delay.h"
#include "zuolan_hmi.h"
#include "Serial.h"
#include "app.h"
#include "menu_task.h"
#include "task_manager.h"

volatile uint8_t gray_sample_req = 0;
volatile uint32_t sys_tick_ms = 0;

float vofa_speed_target = 0.0f; // speed target set via VOFA #P4=xxx!
float vofa_yaw_target = 0.0f;   // yaw target set via VOFA #P4=xxx!
const uint8_t vofa_sent_ch_count = 7;    // number of float channels to send in each VOFA telemetry packet
uint8_t g_angle_mode = 0;      // 0=speed mode, 1=angle mode (toggled via VOFA #P5)

static void on_vofa_param(uint16_t id, float value)
{
    switch (id)
    {
    case 1:
        MotorLSpeedPID.Kp = value;
        MotorRSpeedPID.Kp = value;
        break;
    case 2:
        MotorLSpeedPID.Ki = value;
        MotorRSpeedPID.Ki = value;
        break;
    case 3:
        MotorLSpeedPID.Kd = value;
        MotorRSpeedPID.Kd = value;
        break;
    case 4:
        //vofa_speed_target = value;
        vofa_yaw_target = value;
        break;
    case 5:
        g_angle_mode = value;
        break;
    case 6:
        yaw_pid.Kp = value;
        break;
    case 7:
        yaw_pid.Ki = value;
        break;
    case 8:
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
    case 17:
        vofa_speed_target = value;
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
    vofa_rx_fifo_init();

    AppInit();
    menu_init();
    task_manager_init();

    /* debug: verify code reaches this point via UART0 */
    uart_debug_send_byte('H');
    uart_debug_send_byte('M');
    uart_debug_send_byte('I');
    uart_debug_send_byte('\r');
    uart_debug_send_byte('\n');

    while (1)
    {
        /* ---- VOFA RX: drain FIFO into parser, then apply pending params ---- */
        {
            uint32_t pending = fifo_used(&vofa_rx_fifo);
            while (pending > 0) {
                uint8_t byte;
                if (fifo_read_element(&vofa_rx_fifo, &byte, FIFO_READ_AND_CLEAN) == FIFO_SUCCESS) {
                    vofa_rx_byte(byte);
                }
                pending--;
            }
            vofa_apply_pending();
        }

        /* ---- UART0 heartbeat: confirm main loop is alive (every 500ms) ---- */
        // static uint32_t last_hb = 0;
        // if (sys_tick_ms - last_hb >= 500)
        // {
        //     last_hb = sys_tick_ms;
        //     uart_debug_send_byte('.');
        // }

        /* ---- HMI startup message: one-shot inside main loop ---- */
        static uint8_t hmi_startup_done = 0;
        if (!hmi_startup_done)
        {
            hmi_startup_done = 1;
            zuolan_printf("page1.g0.txt=\"MSPM0 Ready\"%s", HMI_END_CMD);
        }

        SensorProc();

        /* ---- VOFA telemetry: 100ms interval to avoid flooding UART0 ---- */
        static uint32_t last_vofa_telem = 0;
        if (sys_tick_ms - last_vofa_telem >= 100)
        {
            last_vofa_telem = sys_tick_ms;
            float pid_ch[vofa_sent_ch_count] = {
                /* position pid debug */
                vofa_speed_target,      /* ch0: target speed */
                Motor_speedL,
                vofa_yaw_target,        /* ch0: target yaw */
                imu.yaw,                /* ch1: actual yaw */
                // imu.pitch,              /* ch7: pitch deg */
                // imu.roll,               /* ch8: roll deg */
                yaw_pid.Kp,             /* ch2: yaw Kp */
                yaw_pid.Ki,             /* ch3: yaw Ki */
                yaw_pid.Kd,             /* ch4: yaw Kd */
                // (float)gyro_data.z,     /* ch5: raw gyro Z (diagnose SPI) */
                // imu.deg_s.z             /* ch6: filtered gyro Z deg/s */
                
            };
            vofa_send_floats(pid_ch, vofa_sent_ch_count);
        }

        /* ---- HMI serial screen event polling ---- */
        if (hmi_rx_ready)
        {
            hmi_rx_ready = 0;
            uint8_t idx = hmi_rx_idx;
            hmi_rx_idx = 0;  /* reset early so ISR can use fresh buffer */

            /* Inline parse: TJC touch frame = 65 00 pageH pageL widget value */
            if (idx >= 5 && hmi_rx_buf[0] == 0x65)
            {
                uint8_t widget = hmi_rx_buf[3];   /* widget ID maps to task number */
                if (widget >= 1 && widget <= 4)
                {
                    g_current_task = (TaskID)widget;  /* TASK_ONE=1, TASK_TWO=2, ... */
                    menu_active = 0;
                    task_running = 1;
                    g_motor_left_out = 0;
                    g_motor_right_out = 0;
                    zuolan_printf("page1.t%d.txt=\"\xd6\xb4\xd0\xd0\xd6\xd0\"%s", widget - 1, HMI_END_CMD);

                    /* debug: confirm task dispatch via UART0 */
                    uart_debug_send_byte('T');
                    uart_debug_send_byte('0' + widget);
                    uart_debug_send_byte('\r');
                    uart_debug_send_byte('\n');
                }
            }
        }

        /* ---- HMI telemetry: every 200ms send debug data to screen ---- */
        static uint32_t last_hmi_telem = 0;
        if (sys_tick_ms - last_hmi_telem >= 200)
        {
            last_hmi_telem = sys_tick_ms;

            /* motor speed */
            zuolan_HMI_Send_Int("page2.x1", Motor_speedL);
            zuolan_HMI_Send_Int("page2.x2", Motor_speedR);

            /* PID params */
            zuolan_HMI_Send_Float("page2.x3", MotorLSpeedPID.Kp, 2);
            zuolan_HMI_Send_Float("page2.x4", MotorLSpeedPID.Ki, 2);

            /* IMU yaw angle (1 decimal) */
            zuolan_HMI_Send_Float("page2.x5", imu.yaw, 1);

            /* track sensor 8-bit pattern as fixed-width string "11110000" */
            {
                char track_bits[9];
                uint8_t bits = gray_state.state & 0xFF;
                int i;
                for (i = 0; i < 8; i++) {
                    track_bits[7 - i] = ((bits >> i) & 1) ? '1' : '0';
                }
                track_bits[8] = '\0';
                zuolan_HMI_Send_String("page2.tk_string", track_bits);
            }
        }

        /* ---- HMI task completion: clear execution status text ---- */
        static uint8_t was_task_running = 0;
        if (!task_running && was_task_running)
        {
            zuolan_printf("page1.t0.txt=\"\"%s", HMI_END_CMD);
            zuolan_printf("page1.t1.txt=\"\"%s", HMI_END_CMD);
            zuolan_printf("page1.t2.txt=\"\"%s", HMI_END_CMD);
            zuolan_printf("page1.t3.txt=\"\"%s", HMI_END_CMD);
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

        // if (menu_active)
        // {
        //     menu_key_set();
        // }
        // else
        // {
        //     AppProc();
        // }
    }
}

/**
 * TIMER_0 (TIMG0) 1ms tick: key proc, IMU, gray sample request
 */
void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST))
    {
    case DL_TIMER_IIDX_LOAD:
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
        else if (g_angle_mode)
        {
            /* ---- Angle loop: outer yaw PID + inner speed PID ---- */
            ComputeYaw(&yaw_pid, vofa_yaw_target, imu.yaw);

            float angle_out = yaw_pid.CurrentOut;

            /* Differential steering: positive = right turn, negative = left turn */
            float target_l = +angle_out;
            float target_r = -angle_out;

            float abs_target_l = (target_l >= 0.0f) ? target_l : -target_l;
            float abs_target_r = (target_r >= 0.0f) ? target_r : -target_r;
            float abs_L = (Motor_speedL >= 0) ? (float)Motor_speedL : -(float)Motor_speedL;
            float abs_R = (Motor_speedR >= 0) ? (float)Motor_speedR : -(float)Motor_speedR;

            ComputeInc(&MotorLSpeedPID, abs_target_l, abs_L);
            ComputeInc(&MotorRSpeedPID, abs_target_r, abs_R);

            g_motor_left_out  = (target_l >= 0.0f)
                              ? MotorLSpeedPID.CurrentOut
                              : -MotorLSpeedPID.CurrentOut;
            g_motor_right_out = (target_r >= 0.0f)
                              ? MotorRSpeedPID.CurrentOut
                              : -MotorRSpeedPID.CurrentOut;
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
 * GROUP1 = GPIOB + GPIOA interrupt aggregation: encoder quadrature decoding
 *
 * Left  motor encoder: PB6=A-phase, PB5=B-phase  ï¿?????? Count1
 * Right motor encoder: PA29=A-phase, PA30=B-phase ï¿?????? Count2
 *
 * 4x quadrature: both A and B phase edges update the counter.
 * Reference: hal_encode.c in MSPM0G3507_Project_template
 */
void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
    case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
    {
        volatile uint32_t mis = GPIOB->CPU_INT.MIS;
        volatile uint32_t din = GPIOB->DIN31_0;
        uint32_t b_phase = (din >> 5) & 1;   /* PB5 = Encoder1_B */
        uint32_t a_phase = (din >> 6) & 1;   /* PB6 = Encoder1_A */

        if (mis & (1 << 5))   /* B-phase edge */
            Count1 += (b_phase == a_phase) ? 1 : -1;
        if (mis & (1 << 6))   /* A-phase edge */
            Count1 += (a_phase == b_phase) ? -1 : 1;

        GPIOB->CPU_INT.ICLR = (1 << 5) | (1 << 6);
    }
    break;

    case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
    {
        volatile uint32_t mis = GPIOA->CPU_INT.MIS;
        volatile uint32_t din = GPIOA->DIN31_0;
        uint32_t a_phase = (din >> 29) & 1;  /* PA29 = Encoder2_A */
        uint32_t b_phase = (din >> 30) & 1;  /* PA30 = Encoder2_B */

        if (mis & (1 << 29))  /* A-phase edge */
            Count2 += (a_phase == b_phase) ? 1 : -1;
        if (mis & (1 << 30))  /* B-phase edge */
            Count2 += (b_phase == a_phase) ? -1 : 1;

        GPIOA->CPU_INT.ICLR = (1 << 29) | (1 << 30);
    }
    break;

    default:
        break;
    }
}

/**
 * NMI_Handler: sends "NMI!\r\n" via UART0 so we can distinguish from HardFault
 */
void NMI_Handler(void)
{
    uart_debug_send_byte('N');
    uart_debug_send_byte('M');
    uart_debug_send_byte('I');
    uart_debug_send_byte('!');
    uart_debug_send_byte('\r');
    uart_debug_send_byte('\n');
    while (1) { }
}

/**
 * HardFault_Handler: sends "HFLT!\r\n" + stacked PC via UART0 for diagnosis
 */
void HardFault_Handler(void)
{
    uint32_t stacked_pc = 0;
    /* Read stacked PC from exception frame.
     * Cortex-M0+ pushes R0-R3,R12,LR,PC,xPSR onto the faulting stack.
     * Check EXC_RETURN in LR to determine which stack was in use. */
    uint32_t lr;
    __asm volatile ("mov %0, lr" : "=r" (lr));
    uint32_t *frame;
    if (lr & 0x4) {
        frame = (uint32_t *)__get_PSP();
    } else {
        frame = (uint32_t *)__get_MSP();
    }
    stacked_pc = frame[6];  /* PC is 7th word (offset 24) in exception frame */

    uart_debug_send_byte('H');
    uart_debug_send_byte('F');
    uart_debug_send_byte('L');
    uart_debug_send_byte('T');
    uart_debug_send_byte('!');
    /* send stacked PC as 8 hex digits */
    {
        int i;
        for (i = 28; i >= 0; i -= 4) {
            uint8_t nib = (stacked_pc >> i) & 0xFu;
            uart_debug_send_byte(nib < 10 ? '0' + nib : 'A' + nib - 10);
        }
    }
    uart_debug_send_byte('\r');
    uart_debug_send_byte('\n');
    while (1) { }
}
