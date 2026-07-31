#include "hal_qgimbal_can.h"
#include "ti_msp_dl_config.h"
#include "vofa.h"

/* ---- global motor feedback state (volatile: modified in CAN ISR) ---- */
volatile QGimbal_MotorState g_motor_state[QGIMBAL_MOTOR_COUNT] = {
    {0, 0.0f, 0.0f, 0.0f, 0},
    {0, 0.0f, 0.0f, 0.0f, 0},
    {0, 0.0f, 0.0f, 0.0f, 0},
    {0, 0.0f, 0.0f, 0.0f, 0},
};

/* ---- CAN RX debug counters (volatile: written in ISR, read by debugger/main-loop) ---- */
volatile uint32_t qdbg_rx_count     = 0U;   /* total feedback frames received        */
volatile uint32_t qdbg_rx_discard   = 0U;   /* frames discarded (ID/DLC mismatch)    */
volatile uint32_t qdbg_rx_fill_lvl  = 0U;   /* last FIFO fill level                  */
volatile uint16_t qdbg_rx_std_id    = 0U;   /* last received standard CAN ID         */
volatile uint16_t qdbg_rx_angle_raw[QGIMBAL_MOTOR_COUNT] = {0U}; /* per-motor raw angle */

/* ---- scaling constants ---- */
#define QGIMBAL_CURRENT_SCALE  (10.0f)
#define QGIMBAL_SPEED_SCALE    (1000.0f)
#define QGIMBAL_ANGLE_SCALE    (6.283185307f) /* 2*PI */
#define QGIMBAL_INT16_MAX      (32767.0f)
#define QGIMBAL_UINT16_MAX     (65535.0f)

#define QGIMBAL_CAN_LOOPBACK_TEST  0

void QGimbal_CAN_Init(void)
{
    DL_MCAN_StdMsgIDFilterElement filter;

#if QGIMBAL_CAN_LOOPBACK_TEST
    DL_MCAN_setOpMode(MCAN0_INST, DL_MCAN_OPERATION_MODE_SW_INIT);
    while (DL_MCAN_OPERATION_MODE_SW_INIT != DL_MCAN_getOpMode(MCAN0_INST));
    DL_MCAN_lpbkModeEnable(MCAN0_INST, 0U, true);
    DL_MCAN_setOpMode(MCAN0_INST, DL_MCAN_OPERATION_MODE_NORMAL);
    while (DL_MCAN_OPERATION_MODE_NORMAL != DL_MCAN_getOpMode(MCAN0_INST));
#endif

    /* CAN is already in NORMAL mode from SYSCFG_DL_MCAN0_init().
     * Setup filters and interrupts for receiving motor feedback,
     * then send ENABLE commands (same pattern as QGimbal reference). */

    /* Configure standard ID filter 0:
     * Accept IDs 0x500-0x503 (motor 0-3 feedback), store in FIFO0
     * NOTE: sfid1/sfid2 are raw 11-bit IDs �? the driverlib handles
     * the register-level shifting internally (sfid1<<16, sfid2<<0). */
    filter.sfid1 = (uint32_t)(0x500);   /* ID range: 0x500 ~ 0x503 */
    filter.sfid2 = (uint32_t)(0x503);
    filter.sfec  = 1U; /* Store in Rx FIFO 0 if filter matches */
    filter.sft   = 0U; /* Range filter from SFID1 to SFID2 */
    DL_MCAN_addStdMsgIDFilter(MCAN0_INST, 0U, &filter);

    /* Route RX FIFO0 new-message interrupt to line 1 */
    DL_MCAN_selectIntrLine(MCAN0_INST,
        DL_MCAN_INTR_SRC_RX_FIFO0_NEW_MSG, DL_MCAN_INTR_LINE_NUM_1);

    /* Enable RX FIFO0 new-message interrupt source */
    DL_MCAN_enableIntr(MCAN0_INST,
        DL_MCAN_INTR_SRC_RX_FIFO0_NEW_MSG, true);

    /* Enable interrupt line 1 */
    DL_MCAN_enableIntrLine(MCAN0_INST, DL_MCAN_INTR_LINE_NUM_1, true);

    /* Note: NVIC enable for CANFD0_INT_IRQn is in interrupt_config.c */
}

void QGimbal_SendCommand(uint8_t motor_id, QGimbal_Command cmd, int16_t value)
{
    DL_MCAN_TxBufElement txMsg;
    DL_MCAN_ProtocolStatus protStatus;
    uint32_t id;
    volatile uint32_t timeout;

    DL_MCAN_getProtocolStatus(MCAN0_INST, &protStatus);
    if ((DL_MCAN_OPERATION_MODE_NORMAL != DL_MCAN_getOpMode(MCAN0_INST))
        || (1U == protStatus.busOffStatus)) {
        DL_MCAN_txBufCancellationReq(MCAN0_INST, 0U);
        DL_MCAN_setOpMode(MCAN0_INST, DL_MCAN_OPERATION_MODE_NORMAL);
        for (volatile uint32_t d = 0; d < 100; d++) {}
    }

    timeout = 200000U;
    while ((DL_MCAN_getTxBufReqPend(MCAN0_INST) & (1U << 0U)) && (timeout > 0U)) {
        --timeout;
    }

    /* If TX buffer stuck, cancel and recover �? but still send our frame */
    if (timeout == 0U) {
        DL_MCAN_txBufCancellationReq(MCAN0_INST, 0U);
        DL_MCAN_setOpMode(MCAN0_INST, DL_MCAN_OPERATION_MODE_NORMAL);
        for (volatile uint32_t d = 0; d < 100; d++) {}
    }

    /* Build standard ID: 0x400 + motor_id */
    id = 0x400U + (uint32_t)motor_id;

    txMsg.id  = id << 18U;
    txMsg.rtr = 0U;   /* Data frame */
    txMsg.xtd = 0U;   /* 11-bit standard identifier */
    txMsg.esi = 0U;
    txMsg.dlc = 3U;   /* 3 bytes: cmd + value(int16_t) */
    txMsg.brs = 0U;   /* Classic CAN (QD4310 does not support CAN FD) */
    txMsg.fdf = 0U;   /* Classic CAN frame format */
    txMsg.efc = 0U;   /* Don't store Tx events */
    txMsg.mm  = 0U;

    txMsg.data[0] = (uint16_t)cmd;
    txMsg.data[1] = (uint16_t)(value & 0xFF);         /* LSB */
    txMsg.data[2] = (uint16_t)((value >> 8) & 0xFF);  /* MSB */

    /* Write to TX buffer 0 and request transmission */
    DL_MCAN_writeMsgRam(MCAN0_INST, DL_MCAN_MEM_TYPE_BUF, 0U, &txMsg);
    DL_MCAN_TXBufAddReq(MCAN0_INST, 0U);
}

/* ---- high-level motor control ---- */

void QGimbal_Enable(uint8_t motor_id)
{
    QGimbal_SendCommand(motor_id, QGIMBAL_CMD_ENABLE, 0);
}

void QGimbal_Disable(uint8_t motor_id)
{
    QGimbal_SendCommand(motor_id, QGIMBAL_CMD_DISABLE, 0);
}

void QGimbal_SetAngle(uint8_t motor_id, float angle_rad)
{
    int16_t value;
    /* Angle: rad -> uint16_t scaled:
     *   value = (angle_rad / 2pi) * 65535 */
    float scaled = (angle_rad / QGIMBAL_ANGLE_SCALE) * QGIMBAL_UINT16_MAX;
    /* The motor expects the value as an int16_t packed in data bytes.
     * We send the full 16-bit unsigned value cast to int16_t bits. */
    if (scaled > QGIMBAL_UINT16_MAX) scaled = QGIMBAL_UINT16_MAX;
    if (scaled < 0.0f) scaled = 0.0f;
    value = (int16_t)((uint16_t)scaled);  /* pack as uint16 bits */
    QGimbal_SendCommand(motor_id, QGIMBAL_CMD_ANGLE, value);
}

void QGimbal_SetSpeed(uint8_t motor_id, float speed_rpm)
{
    int16_t value;
    /* Speed: rpm -> int16_t scaled:
     *   value = (rpm / 1000) * 32767 */
    float scaled = (speed_rpm / QGIMBAL_SPEED_SCALE) * QGIMBAL_INT16_MAX;
    if (scaled > QGIMBAL_INT16_MAX)  scaled = QGIMBAL_INT16_MAX;
    if (scaled < -QGIMBAL_INT16_MAX) scaled = -QGIMBAL_INT16_MAX;
    value = (int16_t)scaled;
    QGimbal_SendCommand(motor_id, QGIMBAL_CMD_SPEED, value);
}

void QGimbal_SetCurrent(uint8_t motor_id, float current_a)
{
    int16_t value;
    /* Current: A -> int16_t scaled:
     *   value = (current_a / 10.0) * 32767 */
    float scaled = (current_a / QGIMBAL_CURRENT_SCALE) * QGIMBAL_INT16_MAX;
    if (scaled > QGIMBAL_INT16_MAX)  scaled = QGIMBAL_INT16_MAX;
    if (scaled < -QGIMBAL_INT16_MAX) scaled = -QGIMBAL_INT16_MAX;
    value = (int16_t)scaled;
    QGimbal_SendCommand(motor_id, QGIMBAL_CMD_CURRENT, value);
}

void QGimbal_SetZero(uint8_t motor_id)
{
    QGimbal_SendCommand(motor_id, QGIMBAL_CMD_SET_ZERO, 0);
}

void QGimbal_RequestFeedback(uint8_t motor_id)
{
    /* Send NOP (0x00) to poll for feedback without changing motor state.
     * Per QD4310 manual: motor replies with feedback on every received command,
     * so NOP is the softest way to keep angle_rad/speed_rpm current. */
    QGimbal_SendCommand(motor_id, QGIMBAL_CMD_NOP, 0);
}

/* ---- feedback parsing (called from CAN RX ISR) ---- */

void QGimbal_ProcessFeedback(void)
{
    DL_MCAN_RxBufElement rxMsg;
    DL_MCAN_RxFIFOStatus rxFS;

    rxFS.num = DL_MCAN_RX_FIFO_NUM_0;

    /*
     * Batch-drain all pending messages in one ISR entry.
     * This avoids FIFO overflow when multiple motors reply
     * while the ISR was blocked by a same-priority handler.
     */
    for (;;) {
        uint32_t id;
        uint8_t  motor_idx;
        QGimbal_MotorState *motor;

        rxFS.fillLvl = 0;
        DL_MCAN_getRxFIFOStatus(MCAN0_INST, &rxFS);

        if (rxFS.fillLvl == 0) {
            break;  /* FIFO empty — all messages processed */
        }

        qdbg_rx_fill_lvl = rxFS.fillLvl;

        /* Read the oldest message from FIFO0 */
        DL_MCAN_readMsgRam(MCAN0_INST, DL_MCAN_MEM_TYPE_FIFO, 0U, rxFS.num, &rxMsg);

        /* Acknowledge the message */
        DL_MCAN_writeRxFIFOAck(MCAN0_INST, rxFS.num, rxFS.getIdx);

        /* Extract standard ID (bits 28:18) */
        id = (rxMsg.id & 0x1FFC0000U) >> 18U;
        qdbg_rx_std_id = (uint16_t)id;

        /* Dispatch by motor ID (filter accepts 0x500-0x503) */
        if (id < 0x500U || id > 0x503U) {
            qdbg_rx_discard++;
            continue;
        }
        motor_idx = (uint8_t)(id - 0x500U);
        if (motor_idx >= QGIMBAL_MOTOR_COUNT) {
            qdbg_rx_discard++;
            continue;
        }
        motor = (QGimbal_MotorState *)&g_motor_state[motor_idx];

        if (rxMsg.dlc < 8U) {
            qdbg_rx_discard++;  /* Expected 8-byte feedback frame */
            continue;
        }

        /* Parse feedback data:
         *   Data[0]: enabled flag (bit 0)
         *   Data[1]: reserved
         *   Data[2-3]: current (int16_t, scaled: A = raw * 10.0 / 32767)
         *   Data[4-5]: speed   (int16_t, scaled: rpm = raw * 1000.0 / 32767)
         *   Data[6-7]: angle   (uint16_t, scaled: rad = raw * 2pi / 65535) */
        {
            int16_t raw_current = (int16_t)((uint16_t)rxMsg.data[2] | ((uint16_t)rxMsg.data[3] << 8));
            int16_t raw_speed   = (int16_t)((uint16_t)rxMsg.data[4] | ((uint16_t)rxMsg.data[5] << 8));
            uint16_t raw_angle  = (uint16_t)((uint16_t)rxMsg.data[6] | ((uint16_t)rxMsg.data[7] << 8));

            motor->enabled  = (rxMsg.data[0] & 0x01U) ? 1U : 0U;
            motor->current_a = ((float)raw_current / QGIMBAL_INT16_MAX) * QGIMBAL_CURRENT_SCALE;
            motor->speed_rpm = ((float)raw_speed   / QGIMBAL_INT16_MAX) * QGIMBAL_SPEED_SCALE;
            motor->angle_rad = ((float)raw_angle   / QGIMBAL_UINT16_MAX) * QGIMBAL_ANGLE_SCALE;
            motor->feedback_valid = 1U;

            /* Debug: capture raw angle for this motor */
            qdbg_rx_angle_raw[motor_idx] = raw_angle;
        }

        qdbg_rx_count++;
    }
}

/**
 * @brief  CAN diagnostics: send TX/RX status via VOFA JustFloat
 * @note   Call from main loop at low rate (e.g. 500ms)
 *
 *   ch0: tx_count       �? incremented each call (proves function runs)
 *   ch1: TXBTO[0]       �? 1 = last Buffer 0 transmission occurred
 *   ch2: busOffStatus   �? 1 = CAN controller bus-off (dead)
 *   ch3: TEC            �? TX Error Counter
 */
void QGimbal_CAN_Diag(void)
{
    static uint16_t tx_count = 0;
    DL_MCAN_ProtocolStatus protStatus;
    DL_MCAN_ErrCntStatus errCnt;
    float ch[4];

    tx_count++;
    DL_MCAN_getProtocolStatus(MCAN0_INST, &protStatus);
    DL_MCAN_getErrCounters(MCAN0_INST, &errCnt);

    ch[0] = (float)tx_count;
    ch[1] = (float)((DL_MCAN_getTxBufTransmissionStatus(MCAN0_INST) >> 0U) & 1U);
    ch[2] = (float)protStatus.busOffStatus;
    ch[3] = (float)errCnt.transErrLogCnt;

    vofa_send_floats(ch, 4);
}

/**
 * @brief  Motor status telemetry via VOFA JustFloat
 * @note   Call at low rate (e.g. 500ms)
 *
 *   ch0: motor[0] enabled (yaw)
 *   ch1: motor[1] enabled (pitch)
 *   ch2: motor[0] angle_rad
 *   ch3: motor[1] angle_rad
 */
void QGimbal_CAN_Status(void)
{
    float ch[4];

    ch[0] = (float)g_motor_state[0].enabled;
    ch[1] = (float)g_motor_state[1].enabled;
    ch[2] = g_motor_state[0].angle_rad;
    ch[3] = g_motor_state[1].angle_rad;

    vofa_send_floats(ch, 4);
}
