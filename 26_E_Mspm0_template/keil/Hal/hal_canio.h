/**
 * @file hal_canio.h
 * @brief CANIO digital I/O module driver (众盛科技 ZS-DIO series)
 *
 * Controls relay outputs via CAN bus using the CANIO protocol.
 * Standard frame: ID = (func_code << 8) | module_address
 *
 * Function 0x01 (write relay) sends 8 data bytes representing
 * 48 relay channels: data[0] bit0=ch1 ... data[5] bit7=ch48.
 * data[6] and data[7] are reserved.
 *
 * CRITICAL: Function 0x01 overwrites ALL 48 channels. Always
 * maintain a local shadow buffer and send the full 48-channel
 * state when modifying a single channel.
 */
#ifndef __HAL_CANIO_H__
#define __HAL_CANIO_H__

#include <stdint.h>

/* ---- CANIO module address (default = 1) ---- */
#define CANIO_DEVICE_ADDR       1U

/* ---- Function codes ---- */
#define CANIO_FUNC_WRITE_RELAY  0x01U
#define CANIO_FUNC_READ_RELAY   0x02U
#define CANIO_FUNC_READ_INPUT   0x03U
#define CANIO_FUNC_PARAM        0x04U

/* ---- Standard frame CAN ID ---- */
#define CANIO_ID_WRITE_RELAY    ((CANIO_FUNC_WRITE_RELAY << 8U) | CANIO_DEVICE_ADDR)  /* 0x101 */

/* ---- Relay channel connected to electromagnet (1-based, 1~48) ---- */
#define CANIO_RELAY_CHANNEL     1U

/* ---- TX buffer used (Buffer 1, stepper motor disabled) ---- */
#define CANIO_TX_BUF_NUM        1U

/* ---- Baud rate codes (from manual table 2.6) ---- */
#define CANIO_BAUD_CODE_20K     0x02U
#define CANIO_BAUD_CODE_50K     0x03U
#define CANIO_BAUD_CODE_100K    0x04U
#define CANIO_BAUD_CODE_125K    0x05U
#define CANIO_BAUD_CODE_200K    0x06U
#define CANIO_BAUD_CODE_250K    0x07U
#define CANIO_BAUD_CODE_400K    0x08U
#define CANIO_BAUD_CODE_500K    0x09U
#define CANIO_BAUD_CODE_800K    0x0AU
#define CANIO_BAUD_CODE_1000K   0x0BU

/**
 * @brief  Initialize CANIO relay module
 * @note   Sends all-channels-off frame. Call after MCAN0 is in normal mode
 *         and QGimbal_CAN_Init() has completed.
 */
void CANIO_Init(void);

/**
 * @brief  Send full 48-channel relay command frame
 * @param  data : 8-byte relay state array (data[0-5]=ch1-48, data[6-7]=reserved)
 */
void CANIO_WriteRelay(const uint8_t data[8]);

/**
 * @brief  Modify a single relay channel in a shadow buffer
 * @param  data : 8-byte shadow buffer (modified in-place)
 * @param  ch   : channel number (1-based, 1~48)
 * @param  on   : 0 = off, non-zero = on
 * @note   Call CANIO_WriteRelay(data) after modifying to send to module.
 */
void CANIO_SetRelayCh(uint8_t data[8], uint8_t ch, uint8_t on);

#endif /* __HAL_CANIO_H__ */
