#include "Servo.h"
#include <stdint.h>
#include <ti_msp_dl_config.h>

/*
 * Servo PWM reuse TIMA0 (PWM_Motor, SYSCFG_DL_PWM_Motor_init):
 *
 *   TIMA0: BUSCLK / 250 = 320 kHz (1 tick = 3.125 us)
 *   Period: 6400 -> 50 Hz = 20 ms cycle
 *
 *   Edge-align-up CCP action (verified in dl_timer.c:441-446):
 *     ZACT = CCP_HIGH (counter=0 -> output HIGH)
 *     CUACT = CCP_LOW  (counter=CC -> output LOW)
 *   -> HIGH pulse = CC ticks
 *
 *   CC0: PB8  (Servo 1)    CC1: PA7  (Servo 2)
 *   CC2: PB0  (unused)     CC3: PA23 (unused)
 *
 * PWM duty mapping:
 *   0.5 ms = 160 ticks -> CC = 160
 *   2.5 ms = 800 ticks -> CC = 800
 */

#define SERVO_PERIOD            6400
#define SERVO_PULSE_MIN         160
#define SERVO_PULSE_MAX         800
#define SERVO_PULSE_RANGE       ((float)(SERVO_PULSE_MAX - SERVO_PULSE_MIN))
#define SERVO_FULL_ANGLE        180.0f

static void servo_write(uint32_t cc_idx, float angle)
{
    float pulse, cc;
    pulse = (float)SERVO_PULSE_MIN
          + (angle / SERVO_FULL_ANGLE) * SERVO_PULSE_RANGE;
    cc    = pulse;   /* HIGH pulse = CC ticks (ZACT=HIGH, CUACT=LOW) */
    DL_TimerA_setCaptureCompareValue(PWM_Motor_INST,
        (uint32_t)(cc + 0.5f), cc_idx);
}

void Servo_setAngle1(float Angle)   
{
    if (Angle >= (float)SERVO1_MAX_ANGLE) { Angle = (float)SERVO1_MAX_ANGLE; }
    else if (Angle <= (float)SERVO1_MIN_ANGLE) { Angle = (float)SERVO1_MIN_ANGLE; }
    servo_write((uint32_t)GPIO_PWM_Motor_C0_IDX, Angle);
}

void Servo_setAngle2(float Angle)
{
    if (Angle >= (float)SERVO2_MAX_ANGLE) { Angle = (float)SERVO2_MAX_ANGLE; }
    else if (Angle <= (float)SERVO2_MIN_ANGLE) { Angle = (float)SERVO2_MIN_ANGLE; }
    servo_write((uint32_t)GPIO_PWM_Motor_C1_IDX, Angle);
}

/* ---- Raw CC value write (bypasses angle conversion) ---- */
static void servo_write_raw(uint32_t cc_idx, uint32_t cc_value)
{
    DL_TimerA_setCaptureCompareValue(PWM_Motor_INST, cc_value, cc_idx);
}

/* ---- Lift servo: 500us HIGH pulse = 160 ticks -> raise ---- */
void Servo_LiftRaise(void)
{
    servo_write_raw((uint32_t)GPIO_PWM_Motor_C1_IDX, 160);
}

/* ---- Lift servo: 1854us HIGH pulse = 593 ticks -> lower ---- */
void Servo_LiftLower(void)
{
    servo_write_raw((uint32_t)GPIO_PWM_Motor_C1_IDX, 500);
}
