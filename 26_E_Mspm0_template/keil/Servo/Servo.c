#include "Servo.h"
#include <stdint.h>

/*
 * Servo PWM setup (SysConfig required for PWM output):
 *
 * 1. Add a TIMG PWM block named "PWM_Servo" in SysConfig:
 *    - Mode: Edge-aligned up-counting PWM
 *    - Clock prescaler: 1000  (1 tick = 20 us at 50 MHz bus clock)
 *    - Period: 999  (50 Hz = 20 ms cycle)
 *    - CCP0: Servo 1 signal output
 *    - CCP1: Servo 2 signal output
 *    - Assign pins per your hardware (e.g. PA14/PA15)
 * 2. After generating, ti_msp_dl_config.h will define:
 *    PWM_SERVO_INST, PWM_SERVO_C0_IDX, PWM_SERVO_C1_IDX
 *
 * Until SysConfig is set up, angle clamping works but PWM is a no-op.
 */

/* PWM duty mapping (50 Hz, tick = 20 us):
 *   0.5 ms = 25 ticks (0 degrees)
 *   2.5 ms = 125 ticks (180 degrees) */
#define SERVO_PWM_COUNT_MIN    25
#define SERVO_PWM_COUNT_MAX    125
#define SERVO_PWM_COUNT_RANGE  ((float)(SERVO_PWM_COUNT_MAX - SERVO_PWM_COUNT_MIN))
#define SERVO_FULL_ANGLE       180.0f

/*
 * Write angle as PWM duty to a servo channel.
 * When PWM_SERVO_INST is not defined (SysConfig not yet done), this
 * only silences the unused-parameter warning — angle clamping still
 * works so callers do not crash.
 */
static void servo_write(uint32_t cc_idx, float angle)
{
#ifdef PWM_Servo_INST
    float count = (float)SERVO_PWM_COUNT_MIN
                + (angle / SERVO_FULL_ANGLE) * SERVO_PWM_COUNT_RANGE;
    DL_TimerG_setCaptureCompareValue(PWM_Servo_INST,
        (uint32_t)(count + 0.5f), cc_idx);
#else
    (void)cc_idx;
    (void)angle;
#endif
}

void Servo_setAngle1(float Angle)
{
    if (Angle >= (float)SERVO1_MAX_ANGLE) { Angle = (float)SERVO1_MAX_ANGLE; }
    else if (Angle <= (float)SERVO1_MIN_ANGLE) { Angle = (float)SERVO1_MIN_ANGLE; }
    servo_write((uint32_t)PWM_SERVO_C0_IDX, Angle);
}

void Servo_setAngle2(float Angle)
{
    if (Angle >= (float)SERVO2_MAX_ANGLE) { Angle = (float)SERVO2_MAX_ANGLE; }
    else if (Angle <= (float)SERVO2_MIN_ANGLE) { Angle = (float)SERVO2_MIN_ANGLE; }
    servo_write((uint32_t)PWM_SERVO_C1_IDX, Angle);
}
