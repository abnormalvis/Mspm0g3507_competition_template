/**
 * @file arm_task.h
 * @brief SCARA robotic arm pick-and-place task state machine
 *
 * Handshake protocol with RK3588 over UART2:
 *   MSPM0 -> RK3588:  [cmd_byte] + JustFloat arm state
 *   RK3588 -> MSPM0:  JustFloat pick[3] + place[3] angles
 *
 * 1 electromagnet relay (PB13), max 4 fragments per task.
 * Started via VOFA #P52=1!, aborted via #P53=1!.
 */

#ifndef __ARM_TASK_H
#define __ARM_TASK_H

#include <stdint.h>

/* ---- Public API ---- */

void    ArmTask_Init(void);                             /* init static state */
void    ArmTask_Run(void);                              /* 10ms ISR call */
void    ArmTask_Start(void);                            /* begin pick-and-place (chess) */
void    ArmTask_StartPoker(void);                       /* begin pick-and-place (poker cards) */
void    ArmTask_StartTrace(void);                       /* begin shape tracing */
void    ArmTask_Abort(void);                            /* emergency abort */
uint8_t ArmTask_IsActive(void);                         /* returns 1 if running */
uint8_t  ArmTask_HandleVofa(uint16_t id, float value);   /* P52-P53, returns 1 if consumed */
uint32_t ArmTask_GetElapsedSec(void);                     /* elapsed seconds since task start, 0 when idle */

#endif /* __ARM_TASK_H */
