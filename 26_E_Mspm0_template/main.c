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
#include "tracking_loop.h"
#include "hal_qgimbal_can.h"
#include "lora.h"
#include "uart_wired_test.h"
#include "hal_relay.h"

#include "gimbal_control.h"
#include "StepperMotor_CAN.h"
#include "stepper_test.h"

volatile uint8_t gray_sample_req = 0;
volatile uint32_t sys_tick_ms = 0;

float vofa_speed_target = 0.0f; // speed target set via VOFA #P4=xxx!
float vofa_yaw_target = 0.0f;   // yaw target set via VOFA #P4=xxx!
const uint8_t vofa_sent_ch_count = 3;    // number of float channels to send in each VOFA telemetry packet
uint8_t g_angle_mode = 0;      // 0=speed mode, 1=angle mode (toggled via VOFA #P5)
uint8_t g_imu_telem  = 1;      // 1=send IMU telemetry via VOFA (toggled via VOFA #P18)

/* ==== DEBUG: 1 = silence VOFA + HMI telemetry so HMI RX bytes echo cleanly on UART0 ==== */
#define HMI_CAPTURE_DEBUG 0

/* ==== 1 = skip HMI, start line-tracking (task 1) automatically on power-up ==== */
#define AUTO_START_TRACKING 0

/* ---- QGimbal VOFA command layout (base + motor_idx) ---- */
#define VOFA_GIMBAL_ENABLE_BASE 29  /* P29-P32: enable motor 0-3 */
#define VOFA_GIMBAL_SPEED_BASE  33  /* P33-P36: speed target motor 0-3 (rpm) */
#define VOFA_GIMBAL_ANGLE_BASE  37  /* P37-P40: manual angle motor 0-3 (rad) */
#define VOFA_GIMBAL_MANUAL_MODE 41  /* P41: manual angle mode toggle */

static uint8_t vofa_handle_gimbal(uint16_t id, float value)
{
    uint8_t m;

    /* P28: re-enable all motors */
    if (id == 28) {
        for (m = 0; m < GIMBAL_MOTOR_COUNT; m++) {
            QGimbal_Enable(m);
            g_gimbal.enable_retry[m] = 0;
        }
        g_gimbal.motors_enabled = 1;
        return 1;
    }

    /* P29-P32: enable/disable motor 0-3 */
    if (id >= VOFA_GIMBAL_ENABLE_BASE && id <= VOFA_GIMBAL_ENABLE_BASE + 3) {
        m = (uint8_t)(id - VOFA_GIMBAL_ENABLE_BASE);
        if (value != 0.0f) {
            QGimbal_Enable(m);
            g_gimbal.motors_enabled = 1;
            g_gimbal.enable_retry[m] = 0;
        } else { QGimbal_Disable(m); }
        return 1;
    }

    /* P33-P36: speed target motor 0-3 (rpm) */
    if (id >= VOFA_GIMBAL_SPEED_BASE && id <= VOFA_GIMBAL_SPEED_BASE + 3) {
        m = (uint8_t)(id - VOFA_GIMBAL_SPEED_BASE);
        g_gimbal.motor[m].speed_target = value;
        return 1;
    }

    /* P37-P40: manual angle motor 0-3 (rad, 0~2pi) — auto-enters manual mode */
    if (id >= VOFA_GIMBAL_ANGLE_BASE && id <= VOFA_GIMBAL_ANGLE_BASE + 3) {
        m = (uint8_t)(id - VOFA_GIMBAL_ANGLE_BASE);
        g_gimbal.motor[m].angle_manual = value;
        g_gimbal.manual_angle_mode = 1;
        g_gimbal.motors_enabled = 1;
        g_gimbal.enable_retry[m] = 0;
        QGimbal_Enable(m);
        return 1;
    }

    /* P41: manual angle mode toggle (1=enter, 0=exit) */
    if (id == VOFA_GIMBAL_MANUAL_MODE) {
        g_gimbal.manual_angle_mode = (value != 0.0f) ? 1 : 0;
        return 1;
    }

    return 0;
}

static void on_vofa_param(uint16_t id, float value)
{
    if (StepperTest_HandleVofa(id, value)) return;
    if (vofa_handle_gimbal(id, value)) return;
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
        track_pid.in_a = value;
        break;
    case 16:
        track_pid.Kd = value;
        break;
    case 17:
        vofa_speed_target = value;
        break;
    case 18:
        g_imu_telem = (uint8_t)value;
        break;
    default:
        break;
    }
}

int main(void)
{
    SYSCFG_DL_init();
    //hal_delay_init();   /* Explicit SysTick start �???? must precede any delay call */
    gray_init();
    interrupt_init();
    Motor_Init();   /* Set motor driver to initial brake state */
    hal_imu_init();
    SysPidInit();
    EncoderFilterInit();

    vofa_init();
    vofa_set_on_param(on_vofa_param);
    vofa_rx_fifo_init();
    lora_init();
    //UART_WiredTest_Init();

    AppInit();
    menu_init();
    task_manager_init();
    Gimbal_Init();
    // StepperMotor_CAN_Init();
    // StepperTest_Init();
    Relay_Init();

    /* Let the HMI serial screen finish its own power-on boot (~1-2s) before we
     * talk to it, and keep motors idle during this fragile window (task not
     * started yet). Send "Ready" here so it lands after the screen is up. */
    // Delay_ms(2000);
    // zuolan_printf("page1.g0.txt=\"MSPM0 Ready\"%s", HMI_END_CMD);
    // Delay_ms(300);

#if AUTO_START_TRACKING
    /* skip HMI: run line-tracking (task 1) immediately on power-up */
    task_manager_start(TASK_ONE);
#endif

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

        /* ---- LoRa RX: drain FIFO into packet parser ---- */
        lora_rx_drain();

        /* ---- UART_wired test: raw byte echo over wireless link ---- */
        //UART_WiredTest_Run();

        /* ---- HMI startup message: one-shot inside main loop ---- */
        static uint8_t hmi_startup_done = 0;
        if (!hmi_startup_done)
        {
            hmi_startup_done = 1;
            zuolan_printf("page1.g0.txt=\"MSPM0 Ready\"%s", HMI_END_CMD);
        }

        SensorProc();

        /* ---- VOFA telemetry: 100ms interval to avoid flooding UART0 ---- */
#if !HMI_CAPTURE_DEBUG
        // static uint32_t last_vofa_telem = 0;
        // if (sys_tick_ms - last_vofa_telem >= 100)
        // {
        //     last_vofa_telem = sys_tick_ms;
        //     /* tracking-loop PID tuning channels (JustFloat order = VOFA ch0..3) */
        //     float vofa_send_data[vofa_sent_ch_count] = {
        //         // (float)tracking_result.position_error,  /* ch0: track position error (PID input, target 0) */
        //         vofa_speed_target,
        //         // tracking_result.pid_correction,         /* ch1: track PID output (differential correction) */
        //         (float)Motor_speedL,                    /* ch2: left wheel measured speed */
        //         (float)Motor_speedR,                    /* ch3: right wheel measured speed */
        //     };
        //     vofa_send_floats(vofa_send_data, vofa_sent_ch_count);
        // }

        /* ---- IMU telemetry: 200ms interval (VOFA ch0..3 = yaw/pitch/roll/gyro_z) ---- */
        // static uint32_t last_imu_telem = 0;
        // if (g_imu_telem && (sys_tick_ms - last_imu_telem >= 200))
        // {
        //     last_imu_telem = sys_tick_ms;
        //     float imu_ch[4] = {
        //         imu.yaw,            /* ch0: yaw (deg)         */
        //         imu.pitch,          /* ch1: pitch (deg)       */
        //         imu.roll,           /* ch2: roll (deg)        */
        //         imu.deg_s.z,        /* ch3: gyro Z (deg/s)    */
        //     };
        //     vofa_send_floats(imu_ch, 4);
        // }

        /* ---- Stepper motor test: feedback query + VOFA telemetry (200ms) ---- */
#if STEPPER_TEST_ENABLE
        StepperTest_Run();
#endif

        /* ---- QGimbal CAN diagnostics: TX/RX status via VOFA ch4-7 (500ms) ---- */
        // {
        //     static uint32_t last_can_diag = 0;
        //     if (sys_tick_ms - last_can_diag >= 500) {
        //         last_can_diag = sys_tick_ms;
        //         QGimbal_CAN_Diag();
        //         QGimbal_CAN_Status();
        //     }
        // }
#endif

        /* ---- HMI serial screen event polling ---- */
        if (hmi_rx_ready)
        {
            hmi_rx_ready = 0;
            uint8_t idx = hmi_rx_idx;
            hmi_rx_idx = 0;  /* reset early so ISR can use fresh buffer */

            /* Inline parse: HMI touch frame = 65 00 01 <id> <id> 0D 0A  (id at buf[3], 1..4 = task) */
            if (idx >= 4 && hmi_rx_buf[0] == 0x65)
            {
                uint8_t widget = hmi_rx_buf[3];   /* widget/task ID (buf[3]==buf[4]) */
                if (widget >= 1 && widget <= 4)
                {
                    menu_active = 0;
                    task_manager_start((TaskID)widget);  /* 复位状态并�??????????动任�?????????? */
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
#if !HMI_CAPTURE_DEBUG
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

            /* track sensor bit pattern as fixed-width string */
            {
                char track_bits[GRAY_CHANNEL_COUNT + 1];
                int i;
                for (i = 0; i < GRAY_CHANNEL_COUNT; i++) {
                    track_bits[GRAY_CHANNEL_COUNT - 1 - i] =
                        ((gray_state.state >> i) & 1) ? '1' : '0';
                }
                track_bits[GRAY_CHANNEL_COUNT] = '\0';
                zuolan_HMI_Send_String("page2.tk_string", track_bits);
            }
        }
#endif

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
            /* Outer loop: the task (line-tracking) produces LEFT/RIGHT *speed
             * targets* in g_motor_*_out (base ~= task_speed_base = 280). */
            task_manager_run();

            /* Inner loop: convert those speed targets to PWM duty through the
             * already-tuned speed PID -- the SAME path that makes a VOFA speed
             * target actually move the motors. Writing 280 straight to the PWM
             * register is only ~2.8% duty (max_duty=10000), far too weak to
             * turn the wheels, which is why tracking looked "dead". */
            float tgt_l = g_motor_left_out;
            float tgt_r = g_motor_right_out;
            float abs_tl = (tgt_l >= 0.0f) ? tgt_l : -tgt_l;
            float abs_tr = (tgt_r >= 0.0f) ? tgt_r : -tgt_r;
            float abs_L  = (Motor_speedL >= 0) ? (float)Motor_speedL : -(float)Motor_speedL;
            float abs_R  = (Motor_speedR >= 0) ? (float)Motor_speedR : -(float)Motor_speedR;

            ComputeInc(&MotorLSpeedPID, abs_tl, abs_L);
            ComputeInc(&MotorRSpeedPID, abs_tr, abs_R);

            g_motor_left_out  = (tgt_l >= 0.0f) ? MotorLSpeedPID.CurrentOut : -MotorLSpeedPID.CurrentOut;
            g_motor_right_out = (tgt_r >= 0.0f) ? MotorRSpeedPID.CurrentOut : -MotorRSpeedPID.CurrentOut;
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

        /* Gimbal stabilizer control (CAN brushless motors) */
        Gimbal_Run();

        break;
    default:
        break;
    }
}

/**
 * CANFD0 interrupt handler: gimbal + stepper motor feedback reception
 */
void CANFD0_IRQHandler(void)
{
    switch (DL_MCAN_getPendingInterrupt(MCAN0_INST)) {
        case DL_MCAN_IIDX_LINE1: {
            uint32_t status = DL_MCAN_getIntrStatus(MCAN0_INST);
            if (status & DL_MCAN_INTR_SRC_RX_FIFO0_NEW_MSG) {
                QGimbal_ProcessFeedback();
            }
            if (status & DL_MCAN_INTR_SRC_RX_FIFO1_NEW_MSG) {
                StepperMotor_CAN_ProcessFeedback();
            }
            DL_MCAN_clearIntrStatus(MCAN0_INST, status,
                DL_MCAN_INTR_SRC_MCAN_LINE_1);
            break;
        }
        default:
            break;
    }
}

/**
 * GROUP1 = GPIOB + GPIOA interrupt aggregation: encoder quadrature decoding
 *
 * Left  motor encoder: PB6=A-phase, PB5=B-phase  �???????????????? Count1
 * Right motor encoder: PA29=A-phase, PA30=B-phase �???????????????? Count2
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
