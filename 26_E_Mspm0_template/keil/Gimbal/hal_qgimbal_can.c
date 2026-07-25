#include "hal_qgimbal_can.h"
#include "ti_msp_dl_config.h"

/* ---- global motor feedback state ---- */
QGimbal_MotorState g_gimbal_yaw   = {0, 0.0f, 0.0f, 0.0f, 0};
QGimbal_MotorState g_gimbal_pitch = {0, 0.0f, 0.0f, 0.0f, 0};

/* ---- scaling constants ---- */
#define QGIMBAL_CURRENT_SCALE  (10.0f)
#define QGIMBAL_SPEED_SCALE    (1000.0f)
#define QGIMBAL_ANGLE_SCALE    (6.283185307f) /* 2*PI */
#define QGIMBAL_INT16_MAX      (32767.0f)
#define QGIMBAL_UINT16_MAX     (65535.0f)

void QGimbal_CAN_Init(void)
{
    DL_MCAN_StdMsgIDFilterElement filter;

    /* Wait for CAN to enter normal operation mode */
    while (DL_MCAN_OPERATION_MODE_NORMAL != DL_MCAN_getOpMode(MCAN0_INST))
        ;

    /* Configure standard ID filter 0:
     * Accept IDs 0x500-0x501 (yaw/pitch feedback), store in FIFO0 */
    filter.sfid1 = (uint32_t)(0x500) << 18U;
    filter.sfid2 = (uint32_t)(0x501) << 18U;
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
    uint32_t id;

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

/* ---- feedback parsing (called from CAN RX ISR) ---- */

void QGimbal_ProcessFeedback(void)
{
    DL_MCAN_RxBufElement rxMsg;
    DL_MCAN_RxFIFOStatus rxFS;
    uint32_t id;
    QGimbal_MotorState *motor;

    /* Get FIFO status to know which FIFO and get index */
    rxFS.num     = DL_MCAN_RX_FIFO_NUM_0;
    rxFS.fillLvl = 0;
    DL_MCAN_getRxFIFOStatus(MCAN0_INST, &rxFS);

    if (rxFS.fillLvl == 0) {
        return;  /* No message available */
    }

    /* Read the oldest message from FIFO0 */
    DL_MCAN_readMsgRam(MCAN0_INST, DL_MCAN_MEM_TYPE_FIFO, 0U, rxFS.num, &rxMsg);

    /* Acknowledge the message */
    DL_MCAN_writeRxFIFOAck(MCAN0_INST, rxFS.num, rxFS.getIdx);

    /* Extract standard ID (bits 28:18) */
    id = (rxMsg.id & 0x1FFC0000U) >> 18U;

    /* Dispatch by motor ID */
    if (id == 0x500U) {
        motor = &g_gimbal_yaw;
    } else if (id == 0x501U) {
        motor = &g_gimbal_pitch;
    } else {
        return;  /* Unknown ID, ignore */
    }

    if (rxMsg.dlc < 8U) {
        return;  /* Expected 8-byte feedback frame */
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
    }
}
