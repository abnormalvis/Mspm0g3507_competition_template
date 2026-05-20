#include "ti/driverlib/dl_gpio.h"
#include <ti/driverlib/m0p/dl_interrupt.h>
#include "ti_msp_dl_config.h"
#include "hal_encode.h"

ENCODER_RES motor_left_encoder;
ENCODER_RES motor_right_encoder;

volatile int encoder_val_left  = 0;
volatile int encoder_val_right = 0;

/* Zero-valued stubs for legacy path-tracking display code */
float distance_inter = 0, distance_l = 0, distance_r = 0;
float point_actual[2] = {0}, point_A[2] = {0}, point_B[2] = {0}, point_C[2] = {0}, point_D[2] = {0};

void hal_Encoder_Init(void)
{
    encoder_val_left  = 0;
    encoder_val_right = 0;
    motor_left_encoder.temp_count  = 0;
    motor_left_encoder.count  = 0;
    motor_left_encoder.dir    = FORWARD;
    motor_right_encoder.temp_count = 0;
    motor_right_encoder.count = 0;
    motor_right_encoder.dir   = FORWARD;

    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
}

void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
    case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
    {
        volatile uint32_t mis = GPIOB->CPU_INT.MIS;
        volatile uint32_t din = GPIOB->DIN31_0;
        uint32_t a_state = (din >> 5) & 1;  /* PB5 = E1B */
        uint32_t b_state = (din >> 6) & 1;  /* PB6 = E1A */

        if (mis & (1 << 5)) {
            encoder_val_left += (a_state == b_state) ? 1 : -1;
            motor_left_encoder.temp_count += (a_state == b_state) ? 1 : -1;
        }
        if (mis & (1 << 6)) {
            encoder_val_left += (b_state == a_state) ? -1 : 1;
            motor_left_encoder.temp_count += (b_state == a_state) ? -1 : 1;
        }

        GPIOB->CPU_INT.ICLR = (1 << 5) | (1 << 6);
    }
    break;
    case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
    {
        volatile uint32_t mis = GPIOA->CPU_INT.MIS;
        volatile uint32_t din = GPIOA->DIN31_0;
        uint32_t a_state = (din >> 29) & 1; /* PA29 = E2A */
        uint32_t b_state = (din >> 30) & 1; /* PA30 = E2B */

        if (mis & (1 << 29)) {
            encoder_val_right += (a_state == b_state) ? 1 : -1;
            motor_right_encoder.temp_count += (a_state == b_state) ? 1 : -1;
        }
        if (mis & (1 << 30)) {
            encoder_val_right += (b_state == a_state) ? -1 : 1;
            motor_right_encoder.temp_count += (b_state == a_state) ? -1 : 1;
        }

        GPIOA->CPU_INT.ICLR = (1 << 29) | (1 << 30);
    }
    break;
    default:
        break;
    }
}

int Get_Encoder_Count(ENCODER_RES *encoder)
{
    return encoder->count;
}

ENCODER_DIR Get_Encoder_Dir(ENCODER_RES *encoder)
{
    return encoder->dir;
}

void Encoder_Update(ENCODER_RES *encoder)
{
    encoder->count = encoder->temp_count;
    encoder->dir = (encoder->count >= 0) ? FORWARD : REVERSAL;
    encoder->temp_count = 0;
}
