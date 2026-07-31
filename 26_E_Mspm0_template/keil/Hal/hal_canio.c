/**
 * @file hal_canio.c
 * @brief CANIO digital I/O module driver implementation
 *
 * Sends relay write commands to a 众盛科技 ZS-DIO CANIO module
 * via CANFD0 (MCAN0). Uses TX Buffer 1 (shared with stepper motor
 * CAN �? ensure StepperMotor_CAN_Init() is NOT called when CANIO
 * relay is active).
 *
 * Standard frame format:
 *   ID  = (func_code << 8) | module_addr
 *   DLC = 8
 *   Data[0-5] = relay ch1-48 state (LSB per byte = lowest channel)
 *   Data[6-7] = reserved (0)
 */
#include "hal_canio.h"
#include "ti_msp_dl_config.h"

/**
 * @brief  Wait for TX buffer to become free, with timeout recovery
 * @param  buf_num : TX buffer number (0 or 1)
 */
static void CANIO_WaitTxBuf(uint8_t buf_num)
{
    volatile uint32_t timeout = 200000U;
    while ((DL_MCAN_getTxBufReqPend(MCAN0_INST) & (1U << buf_num)) && (timeout > 0U)) {
        --timeout;
    }
    if (timeout == 0U) {
        DL_MCAN_txBufCancellationReq(MCAN0_INST, buf_num);
        DL_MCAN_setOpMode(MCAN0_INST, DL_MCAN_OPERATION_MODE_NORMAL);
        for (volatile uint32_t d = 0; d < 100; d++) {}
    }
}

/**
 * @brief  Ensure MCAN0 is in normal operation mode
 */
static void CANIO_EnsureNormalMode(void)
{
    DL_MCAN_ProtocolStatus protStatus;
    DL_MCAN_getProtocolStatus(MCAN0_INST, &protStatus);
    if ((DL_MCAN_OPERATION_MODE_NORMAL != DL_MCAN_getOpMode(MCAN0_INST))
        || (1U == protStatus.busOffStatus)) {
        DL_MCAN_txBufCancellationReq(MCAN0_INST, CANIO_TX_BUF_NUM);
        DL_MCAN_setOpMode(MCAN0_INST, DL_MCAN_OPERATION_MODE_NORMAL);
        for (volatile uint32_t d = 0; d < 100; d++) {}
    }
}

void CANIO_Init(void)
{
    uint8_t all_off[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /* Wait for CAN to enter normal mode (may still be in init from
     * SYSCFG_DL_init). Bus-off recovery if needed. */
    {
        volatile uint32_t can_timeout = 1000000U;
        while ((DL_MCAN_OPERATION_MODE_NORMAL != DL_MCAN_getOpMode(MCAN0_INST))
               && (can_timeout > 0U)) {
            --can_timeout;
        }
    }

    /* Send all-relays-off command to initialize the module to a known state */
    CANIO_WriteRelay(all_off);
}

void CANIO_WriteRelay(const uint8_t data[8])
{
    DL_MCAN_TxBufElement txMsg;
    uint8_t i;

    CANIO_EnsureNormalMode();
    CANIO_WaitTxBuf(CANIO_TX_BUF_NUM);

    /* Build standard 11-bit ID frame */
    txMsg.id  = ((uint32_t)CANIO_ID_WRITE_RELAY) << 18U;
    txMsg.rtr = 0U;   /* Data frame */
    txMsg.xtd = 0U;   /* 11-bit standard identifier */
    txMsg.esi = 0U;
    txMsg.dlc = 8U;   /* 8 data bytes */
    txMsg.brs = 0U;   /* Classic CAN rate */
    txMsg.fdf = 0U;   /* Classic CAN frame format */
    txMsg.efc = 0U;   /* Don't store Tx events */
    txMsg.mm  = 0U;

    /* Copy all 8 data bytes */
    for (i = 0U; i < 8U; i++) {
        txMsg.data[i] = data[i];
    }

    /* Write to TX buffer and request transmission */
    DL_MCAN_writeMsgRam(MCAN0_INST, DL_MCAN_MEM_TYPE_BUF, CANIO_TX_BUF_NUM, &txMsg);
    DL_MCAN_TXBufAddReq(MCAN0_INST, CANIO_TX_BUF_NUM);
}

void CANIO_SetRelayCh(uint8_t data[8], uint8_t ch, uint8_t on)
{
    uint8_t byte_idx;
    uint8_t bit_mask;

    if (ch < 1U || ch > 48U) {
        return;  /* Invalid channel */
    }

    /* Channel 1 = data[0] bit0, channel 8 = data[0] bit7,
     * channel 9 = data[1] bit0, ..., channel 48 = data[5] bit7 */
    byte_idx = (ch - 1U) >> 3U;   /* (ch-1) / 8 */
    bit_mask = (uint8_t)(1U << ((ch - 1U) & 0x07U));  /* 1 << ((ch-1) % 8) */

    if (on) {
        data[byte_idx] |= bit_mask;
    } else {
        data[byte_idx] &= (uint8_t)(~bit_mask);
    }
}
