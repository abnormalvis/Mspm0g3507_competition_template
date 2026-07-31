/**
 * @file telemetry_proto.c
 * @brief Custom telemetry protocol implementation
 *
 * Builds frames with format: | 0xA5 | LEN | float32_LE_data... | XOR_CKSUM |
 * Sends the same frame on both UART0 (VOFA debug) and UART2 (RK3588).
 */

#include "telemetry_proto.h"
#include "hal_qgimbal_can.h"   /* g_motor_state[] */
#include "arm_control.h"       /* arm_uart2_send_byte(), arm_uart2_send_bytes() */
#include "arm_protocol.h"      /* ARM_MOTOR_COUNT */
#include "vofa.h"              /* vofa_send_floats() �? JustFloat for VOFA display */

/* ================================================================ */
/*  XOR checksum                                                     */
/* ================================================================ */

uint8_t Telemetry_ComputeXor(const uint8_t *data, uint8_t len)
{
    uint8_t ck = 0;
    uint8_t i;
    for (i = 0; i < len; i++) {
        ck ^= data[i];
    }
    return ck;
}

/* ================================================================ */
/*  Frame builder                                                    */
/* ================================================================ */

/**
 * @brief Pack one float32 into a byte buffer (little-endian, no stdlib).
 */
static void telem_pack_float(uint8_t *buf, float v)
{
    union { float f; uint32_t u; } conv;
    conv.f = v;
    buf[0] = (uint8_t)(conv.u & 0xFFU);
    buf[1] = (uint8_t)((conv.u >> 8U) & 0xFFU);
    buf[2] = (uint8_t)((conv.u >> 16U) & 0xFFU);
    buf[3] = (uint8_t)((conv.u >> 24U) & 0xFFU);
}

/**
 * @brief Build and send a telemetry frame on both UART0 and UART2.
 *
 * @param floats     pointer to float array
 * @param count      number of floats
 * @param data_len   total data bytes (= count * 4)
 */
static void telem_send_frame(const float *floats, uint8_t count, uint8_t data_len)
{
    uint8_t buf[2 + TELEM_MAX_DATA + 1];   /* header + len + data + checksum */
    uint8_t i;
    uint8_t ck;

    /* Header */
    buf[0] = TELEM_HEADER;

    /* Length (data bytes only) */
    buf[1] = data_len;

    /* Data (float32 LE) */
    for (i = 0; i < count; i++) {
        telem_pack_float(&buf[2 + i * 4], floats[i]);
    }

    /* Checksum (XOR over [header, len, data]) */
    ck = Telemetry_ComputeXor(buf, (uint8_t)(2U + data_len));
    buf[2 + data_len] = ck;

    /* Total frame size */
    {
        uint8_t total = (uint8_t)(3U + data_len);   /* header(1) + len(1) + data + ck(1) */

        /* Send on UART0 (VOFA debug) �? use JustFloat format so VOFA can display waveforms */
        vofa_send_floats(floats, count);

        /* Send on UART2 (RK3588) �? custom 0xA5 protocol with XOR checksum */
        arm_uart2_send_bytes(buf, (uint16_t)total);
    }
}

/* ================================================================ */
/*  P68 / P69 public API                                             */
/* ================================================================ */

void Telemetry_SendAngles(void)
{
    float angles[TELEM_ANGLES_FLOAT_COUNT];
    uint8_t i;

    for (i = 0; i < TELEM_ANGLES_FLOAT_COUNT; i++) {
        angles[i] = g_motor_state[i].angle_rad;
    }

    telem_send_frame(angles, TELEM_ANGLES_FLOAT_COUNT, TELEM_ANGLES_DATA_LEN);
}

void Telemetry_SendMotorState(void)
{
    float state[TELEM_STATE_FLOAT_COUNT];
    uint8_t i;

    for (i = 0; i < ARM_MOTOR_COUNT; i++) {
        state[i * 3 + 0] = g_motor_state[i].angle_rad;
        state[i * 3 + 1] = g_motor_state[i].speed_rpm;
        state[i * 3 + 2] = g_motor_state[i].current_a;
    }

    telem_send_frame(state, TELEM_STATE_FLOAT_COUNT, TELEM_STATE_DATA_LEN);
}
