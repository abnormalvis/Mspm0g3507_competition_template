//
// hal_vofa.h
// VOFA+ protocol parser - receives #P<id>=<value>! format
//
#ifndef VOFA_H
#define VOFA_H

#include "stdint.h"
#include "hal_motor.h"

#define VOFA_PARAM_MAX  20

/* VOFA parameter ID definitions - can be freely mapped to any variable */
/* Speed loop PID - shared for both motors */
#define VOFA_PARAM_SPEED_KP      1
#define VOFA_PARAM_SPEED_KI     2
#define VOFA_PARAM_SPEED_KD     3
#define VOFA_PARAM_SPEED_TARGET 4
/* Angle loop PID */
#define VOFA_PARAM_ANGLE_KP     5
#define VOFA_PARAM_ANGLE_KI     6
#define VOFA_PARAM_ANGLE_KD     7
#define VOFA_PARAM_ANGLE_TARGET 8
/* Seektrack loop PID */
#define VOFA_PARAM_SEEK_KP      9
#define VOFA_PARAM_SEEK_KI      10
#define VOFA_PARAM_SEEK_KD      11
#define VOFA_PARAM_SEEK_TARGET  12
#define VOFA_PARAM_WHEEL_RADIUS 13
/* Position loop PID */
#define VOFA_PARAM_POSITION_KP   15
#define VOFA_PARAM_POSITION_KI   16
#define VOFA_PARAM_POSITION_KD   17
#define VOFA_PARAM_POSITION_TARGET 18

/**
 * VOFA parameter mapping macro
 * Maps parameter ID to target variable pointer
 * Usage: XX(id, pointer)
 * Note: Same parameter can map to multiple variables (e.g., both motors)
 */
#define VOFA_PARAM_MAP(XX) \
    XX(VOFA_PARAM_SPEED_KP,      &motor_left.speed_ctrl.kp)   \
    XX(VOFA_PARAM_SPEED_KP,      &motor_right.speed_ctrl.kp)  \
    XX(VOFA_PARAM_SPEED_KI,      &motor_left.speed_ctrl.ki)   \
    XX(VOFA_PARAM_SPEED_KI,      &motor_right.speed_ctrl.ki)  \
    XX(VOFA_PARAM_SPEED_KD,      &motor_left.speed_ctrl.kd)   \
    XX(VOFA_PARAM_SPEED_KD,      &motor_right.speed_ctrl.kd)  \
    XX(VOFA_PARAM_SPEED_TARGET,  &motor_left.target)         \
    XX(VOFA_PARAM_SPEED_TARGET,  &motor_right.target)        \
    XX(VOFA_PARAM_WHEEL_RADIUS,  &wheel_radius_cm)

/* Parameter update notification callback */
typedef void (*vofa_param_callback_t)(uint8_t id, float value);

/**
 * Initialize VOFA parser
 */
void vofa_param_init(void);

/**
 * Register callback for parameter updates
 * callback: function to call when parameter is updated
 */
void vofa_register_param_callback(vofa_param_callback_t callback);

/**
 * Check if any parameter has been updated since last call
 * Returns: 1 if update available, 0 otherwise
 */
uint8_t vofa_has_param_update(void);

/**
 * Get the ID of the last updated parameter
 * Returns: parameter ID (1-20) or 0 if no update
 * Note: also clears the update flag
 */
uint8_t vofa_get_param_id(void);

/**
 * Peek at the last updated parameter ID without clearing flag
 * Returns: parameter ID (1-20) or 0 if no update
 */
uint8_t vofa_peek_param_id(void);

/**
 * Get the value of a specific parameter
 * id: parameter ID (1-20)
 * Returns: parameter value, or 0 if invalid ID
 */
float vofa_get_param_value(uint8_t id);

/**
 * UART receive callback - called from UART ISR
 * byte: received byte
 */
void vofa_uart_rx_callback(uint8_t byte);

/**
 * Apply a single parameter to all its mapped targets
 * id: parameter ID
 * value: new value
 */
void vofa_apply_param(uint8_t id, float value);

/**
 * VOFA 10ms task - processes received parameters
 * Should be called from TIMG0_IRQHandler every 10ms
 */
void vofa_task_10ms(void);

/**
 * Lightweight UART poll - reads from debug_uart_fifo and forwards to VOFA parser
 * Replaces uartwork() to avoid seekfree assistant overhead
 */
void vofa_uart_poll(void);

#endif //VOFA_H