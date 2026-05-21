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

volatile uint8_t  gray_sample_req   = 0;
volatile uint32_t sys_tick_ms       = 0;

static void on_vofa_param(uint16_t id, float value)
{
    switch (id)
    {
        case 1: MotorLSpeedPID.Kp = value;     break;
        case 2: MotorLSpeedPID.Ki = value;     break;
        case 3: MotorLSpeedPID.Kd = value;     break;
        case 4: MotorRSpeedPID.Kp = value;     break;
        case 5: MotorRSpeedPID.Ki = value;     break;
        case 6: MotorRSpeedPID.Kd = value;     break;
        case 7: yaw_pid.Kp        = value;     break;
        case 8: yaw_pid.Ki        = value;     break;
        case 9: yaw_pid.Kd        = value;     break;
        case 10: track_pid.Kp     = value;     break;
        case 11: track_pid.Ki     = value;     break;
        case 12: track_pid.Kd     = value;     break;
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

    while (1)
    {
        AppProc();
    }
}

/**
 * TIMER_0 (TIMA1) 1ms tick: 10ms key proc, 5ms IMU, 5ms gray sample request
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
            if ((sys_tick_ms % 10) == 0) EncoderGetValue();
            break;
        default:
            break;
    }
}

/**
 * GROUP1 = GPIOB 中断聚合：键盘下降沿 + Encoder1 A 相边沿
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
