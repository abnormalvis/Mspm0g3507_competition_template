/**
 * @file telemetry_proto.h
 * @brief Custom telemetry protocol: frame header + length + data + XOR checksum
 *
 * Frame format:
 *   | 0xA5 | LEN | DATA[0..LEN-1] | XOR_CKSUM |
 *
 *   - 0xA5 : frame header (1 byte)
 *   - LEN   : data byte count, excluding header/length/checksum (1 byte)
 *   - DATA  : float32 little-endian payload
 *   - CKSUM : XOR of [0xA5, LEN, DATA[0..LEN-1]] (1 byte)
 *
 * Two telemetry commands:
 *   P68 = 3 motor angles (12 bytes data, 15 byte frame)
 *   P69 = 3 motors x (angle+speed+current) (36 bytes data, 39 byte frame)
 *
 * Each frame is sent on BOTH UART0 (VOFA debug) and UART2 (RK3588).
 */

#ifndef __TELEMETRY_PROTO_H
#define __TELEMETRY_PROTO_H

#include <stdint.h>

/* ---- Frame constants ---- */
#define TELEM_HEADER        0xA5U
#define TELEM_MAX_DATA      64    /* max payload bytes (1-byte length field => 255 max) */

/* ---- P68 payload: 3 floats = 12 bytes ---- */
#define TELEM_ANGLES_FLOAT_COUNT  3
#define TELEM_ANGLES_DATA_LEN     (TELEM_ANGLES_FLOAT_COUNT * 4U)   /* 12 */

/* ---- P69 payload: 9 floats = 36 bytes ---- */
#define TELEM_STATE_FLOAT_COUNT   9
#define TELEM_STATE_DATA_LEN      (TELEM_STATE_FLOAT_COUNT * 4U)   /* 36 */

/* ---- Public API ---- */

/**
 * @brief Send P68 frame: 3 motor angles via custom protocol on UART0 + UART2.
 */
void Telemetry_SendAngles(void);

/**
 * @brief Send P69 frame: 3 motors full state (angle/speed/current) on UART0 + UART2.
 */
void Telemetry_SendMotorState(void);

/**
 * @brief Compute XOR checksum over a buffer.
 * @param data  buffer to checksum
 * @param len   number of bytes
 * @return XOR of all bytes
 */
uint8_t Telemetry_ComputeXor(const uint8_t *data, uint8_t len);

#endif /* __TELEMETRY_PROTO_H */
