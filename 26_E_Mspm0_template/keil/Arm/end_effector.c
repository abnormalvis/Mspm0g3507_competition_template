/**
 * @file end_effector.c
 * @brief End-effector (electromagnet + lift servo) — direct blocking control
 *
 * P66 pick sequence (runs in main loop context with IRQs enabled):
 *   1. Servo_LiftLower() — spin-wait ARMP_LIFT_DELAY_MS
 *   2. Relay_On()        — spin-wait ARMP_PICK_DELAY_MS
 *   3. Servo_LiftRaise() — spin-wait ARMP_LIFT_DELAY_MS
 *
 * P67 place: Relay_Off() immediately.
 *
 * The old ISR-based state machine has been removed.  The blocking
 * sequence is simpler and avoids all races between main loop and
 * TIMER_1 ISR that plagued the previous two rounds of fixes.
 */

#include "end_effector.h"
#include "arm_protocol.h"    /* ARMP_LIFT_DELAY_MS, ARMP_PICK_DELAY_MS */
#include "Servo.h"
#include "hal_relay.h"

/* ---- sys_tick_ms (declared in main.c, incremented in TIMER_0 ISR) ---- */
extern volatile uint32_t sys_tick_ms;

/* ================================================================ */
/*  Public API                                                       */
/* ================================================================ */

void EndEffector_Init(void)
{
    /* No state to initialise — pick runs inline in HandleVofa. */
}

void EndEffector_Run(void)
{
    /* No-op: the pick sequence executes inline in EndEffector_HandleVofa
     * (main loop context), not here in the 10ms TIMER_1 ISR. */
}

uint8_t EndEffector_IsBusy(void)
{
    return 0;   /* always ready — sequence is blocking, never concurrent */
}

void EndEffector_Abort(void)
{
    Relay_Off();
}

uint8_t EndEffector_HandleVofa(uint16_t id, float value)
{
    /* P66: pick — hard-coded blocking sequence (IRQs enabled) */
    if (id == 66 && value != 0.0f) {
        volatile uint32_t start;

        /* Phase 1: lower lift servo, hold ARMP_LIFT_DELAY_MS */
        Servo_LiftLower();
        start = sys_tick_ms;
        while ((sys_tick_ms - start) < ARMP_LIFT_DELAY_MS);

        /* Phase 2: energize electromagnet (relay on), hold ARMP_PICK_DELAY_MS */
        Relay_On();
        start = sys_tick_ms;
        while ((sys_tick_ms - start) < ARMP_PICK_DELAY_MS);

        /* Phase 3: raise lift servo, hold ARMP_LIFT_DELAY_MS */
        Servo_LiftRaise();
        start = sys_tick_ms;
        while ((sys_tick_ms - start) < ARMP_LIFT_DELAY_MS);

        return 1;
    }

    /* P67: place (relay off immediately) */
    if (id == 67 && value != 0.0f) {
        Relay_Off();
        return 1;
    }

    return 0;  /* ID not consumed */
}
