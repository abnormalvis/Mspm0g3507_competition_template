/**
 * @file arm_protocol.h
 * @brief MSPM0-RK3588 handshake protocol constants
 *
 * Shared between arm_control (low-level driver) and arm_task (high-level
 * pick-and-place state machine).
 *
 * Frame format (MSPM0 -> RK3588):  [cmd_byte] [6xfloat LE] [00 00 80 7F]
 * Frame format (RK3588 -> MSPM0):  [6xfloat LE] [00 00 80 7F]
 */

#ifndef __ARM_PROTOCOL_H
#define __ARM_PROTOCOL_H

#include <stdint.h>

/* ---- Commands: MSPM0 -> RK3588 ---- */
#define ARMP_CMD_TASK_START     0x01u  /* start chess pick-and-place */
#define ARMP_CMD_CONTINUE       0x02u
#define ARMP_CMD_TASK_COMPLETE  0x03u
#define ARMP_CMD_TRACE          0x04u  /* start shape tracing mode */
#define ARMP_CMD_POKER_START    0x05u  /* start poker card pick-and-place */

/* ---- Frame sizes ---- */
#define ARMP_TX_FLOAT_COUNT  6
#define ARMP_TX_DATA_BYTES   (ARMP_TX_FLOAT_COUNT * 4)   /* 24 */
#define ARMP_TX_FRAME_BYTES  (1 + ARMP_TX_DATA_BYTES + 4) /* 29 = cmd + floats + tail */

/* ---- Sentinel: RK3588 -> MSPM0 "no more fragments" ---- */
#define ARMP_SENTINEL_NO_FRAGMENTS  (-1.0f)

/* ---- Task configuration ---- */
#define ARMP_MAX_FRAGMENTS          4
#define ARMP_MOVE_DEADBAND_RAD      0.035f   /* radians: considered "arrived" (≈2.0°) */
#define ARMP_PICK_DELAY_MS          500      /* ms: magnet energize settle */
#define ARMP_PLACE_DELAY_MS         500      /* ms: magnet release settle */
#define ARMP_LIFT_DELAY_MS          500      /* ms: servo lift settle */
#define ARMP_RK3588_TIMEOUT_MS      5000     /* ms: give up waiting for RK3588 */
#define ARMP_ARRIVAL_TIMEOUT_MS     3000     /* ms: give up waiting for motor arrival */
#define ARMP_ZERO_ANGLE_RAD         0.0f     /* home position for all motors (rad) */

/* ---- Math ---- */
#define ARMP_PI_F          3.1415926f
#define ARMP_TWO_PI_F      (ARMP_PI_F * 2.0f)
#define ARMP_DEG2RAD(d)    ((d) * ARMP_PI_F / 180.0f)
#define ARMP_RAD2DEG(r)    ((r) * 180.0f / ARMP_PI_F)


/* ---- States for the arm task state machine ---- */
typedef enum {
    ARMP_IDLE = 0,
    ARMP_SEND_START,
    ARMP_WAIT_TARGET,
    ARMP_MOVE_TO_PICK,
    ARMP_WAIT_PICK_ARRIVAL,
    ARMP_PICK,
    ARMP_MOVE_TO_PLACE,
    ARMP_WAIT_PLACE_ARRIVAL,
    ARMP_PLACE,
    ARMP_LIFT_DOWN,         /* lower servo before pick/place */
    ARMP_LIFT_UP,           /* raise servo after pick/place */
    ARMP_SEND_CONTINUE,
    ARMP_RETURN_ZERO,
    ARMP_WAIT_ZERO_ARRIVAL,
    ARMP_SEND_COMPLETE,
    ARMP_ERROR,
    /* ---- Trace mode states ---- */
    ARMP_TRACE_MOVE_TO_WAYPOINT,
    ARMP_TRACE_WAIT_ARRIVAL,
    ARMP_TRACE_SEND_CONTINUE
} ArmTaskState;

/* ---- Per-task context (static in arm_task.c) ---- */
typedef struct {
    ArmTaskState state;
    uint8_t      fragment_index;
    float        pick_angles[3];
    float        place_angles[3];
    uint32_t     state_enter_ms;
    uint8_t      tx_buf[ARMP_TX_FRAME_BYTES];
    uint32_t     task_start_ms;   /* sys_tick_ms when ArmTask_Start() was called */
    uint8_t      trace_mode;      /* 1 = trace mode active, 0 = pick-and-place */
    ArmTaskState lift_next_state; /* next state after lift completes */
} ArmTaskContext;

#endif /* __ARM_PROTOCOL_H */
