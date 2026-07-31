/**
 * @file end_effector.h
 * @brief End-effector (electromagnet + lift servo) VOFA command module
 *
 * Independent of the arm_task pick-and-place FSM.
 * Provides two VOFA commands:
 *   P66 = pick  (servo lower -> relay on -> servo raise, ~1.5s blocking)
 *   P67 = place (relay off, instantaneous)
 *
 * The pick sequence is a hard-coded blocking sequence inside
 * EndEffector_HandleVofa(), which runs in main loop context with
 * interrupts enabled.  EndEffector_Run() is a no-op (kept for API
 * compatibility with Arm_Run()).
 */

#ifndef __END_EFFECTOR_H
#define __END_EFFECTOR_H

#include <stdint.h>

/* ---- Public API ---- */

void    EndEffector_Init(void);                              /* no-op (kept for API compat) */
void    EndEffector_Run(void);                               /* no-op (pick runs in HandleVofa) */
uint8_t EndEffector_HandleVofa(uint16_t id, float value);    /* P66 blocking, P67 immediate */
uint8_t EndEffector_IsBusy(void);                            /* always returns 0 */
void    EndEffector_Abort(void);                             /* relay off immediately */

#endif /* __END_EFFECTOR_H */
