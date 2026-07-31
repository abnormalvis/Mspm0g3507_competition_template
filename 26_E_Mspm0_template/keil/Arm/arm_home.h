/**
 * @file arm_home.h
 * @brief Auto-home module â€? returns arm to workpiece coordinate on power-up
 *
 * On startup, after a safety delay, enables all 3 motors and moves them
 * to the pre-configured workpiece coordinate angles.
 *
 * Angles are in RADIANS (consistent with QD4310 host and arm_control).
 *
 * Default angles can be updated via VOFA P62 (save to internal flash)
 * and reset via P63 (revert to compile-time defaults).
 */

#ifndef __ARM_HOME_H
#define __ARM_HOME_H

#include <stdint.h>

/* ---- Compile-time feature toggle ---- */
#define AUTO_HOME_ON_STARTUP 1

/* ---- Default workpiece coordinate angles (radians) ---- */
#define AUTOHOME_DEFAULT_M0_RAD  5.24f
#define AUTOHOME_DEFAULT_M1_RAD  1.69f
#define AUTOHOME_DEFAULT_M2_RAD  0.0f

/* ---- Timing ---- */
#define AUTOHOME_POWERUP_DELAY_MS   3000   /* safety delay after boot */
#define AUTOHOME_ARRIVAL_TIMEOUT_MS 5000   /* wait-for-arrival timeout */

/* ---- State machine (independent of arm_task pick-and-place FSM) ---- */
typedef enum {
    AUTOHOME_IDLE = 0,
    AUTOHOME_WAIT_DELAY,       /* waiting for power-up safety delay */
    AUTOHOME_START_MOTORS,     /* enable motors + direct CAN angle send (atomic) */
    AUTOHOME_WAIT_ARRIVAL,     /* polling arm_is_at_target() */
    AUTOHOME_DONE,             /* home complete â€? motors stay enabled */
    AUTOHOME_ERROR             /* timeout â€? finish gracefully */
} AutoHomeState;

/* ---- Context (static singleton in arm_home.c) ---- */
typedef struct {
    AutoHomeState state;
    uint32_t      state_enter_ms;    /* sys_tick_ms at state entry */
    float         angles_rad[3];     /* workpiece coordinate angles (rad) */
    uint8_t       home_performed;    /* 1 = already homed this power cycle */
} AutoHomeContext;

/* ---- Public API ---- */

void    ArmHome_Init(void);
void    ArmHome_Run(void);                               /* 10ms ISR call, runs inside Arm_Run() */
uint8_t ArmHome_HandleVofa(uint16_t id, float value);    /* P62=save, P63=clear */
uint8_t ArmHome_IsActive(void);

#endif /* __ARM_HOME_H */
