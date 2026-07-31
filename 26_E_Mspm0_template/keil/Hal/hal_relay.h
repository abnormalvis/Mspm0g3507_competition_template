/**
 * @file hal_relay.h
 * @brief CANIO relay driver (CANIO digital I/O module)
 *
 * Controls a single relay (electromagnet) via the CANIO protocol over CANFD0.
 * The relay hardware is connected to a 众盛科技 ZS-DIO CANIO module on the
 * CAN bus. This driver wraps the low-level CANIO driver (hal_canio.h) and
 * exposes the same API as the original GPIO-based relay driver, so all
 * callers (arm_task, arm_control, main) remain unchanged.
 */
#ifndef __HAL_RELAY_H__
#define __HAL_RELAY_H__

#include "hal_canio.h"

/* ---- Relay channel on CANIO module (1-based, 1~48) ---- */
#define CANIO_RELAY_CH  CANIO_RELAY_CHANNEL

void Relay_Init(void);
void Relay_On(void);
void Relay_Off(void);
void Relay_Toggle(void);
uint8_t Relay_GetState(void);

#endif /* __HAL_RELAY_H__ */
