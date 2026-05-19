/**
 * hal_motor.h
 * Motor control abstraction layer - unified management of left/right motor structures
 */
#ifndef _HAL_MOTOR_H
#define _HAL_MOTOR_H

#include "stdint.h"
#include "hal_pid.h"

/**
 * Motor structure
 */
typedef struct {
    float target;       /* Target speed in cm/s */
    float feedback;     /* Actual speed in cm/s */
    float output;       /* PWM output */
    controller speed_ctrl;  /* Speed loop PID controller */
} Motor_t;

/* Left/right motor global variables */
extern Motor_t motor_left;
extern Motor_t motor_right;

/**
 * Motor initialization
 */
void motor_init(void);

/**
 * Set motor target speed
 * left: Left wheel target speed in cm/s
 * right: Right wheel target speed in cm/s
 */
void motor_set_target(float left, float right);

/**
 * Motor speed loop PID control calculation
 * After calling this function, results are stored in motor_left.output and motor_right.output
 */
void motor_control(void);

/**
 * Direct motor PWM output (includes direction control)
 * left: Left wheel PWM (positive=forward, negative=backward, 0=stop)
 * right: Right wheel PWM (positive=forward, negative=backward, 0=stop)
 */
void motor_set_pwm(int left, int right);

/**
 * Get left wheel actual speed in cm/s
 */
float motor_get_left_speed(void);

/**
 * Get right wheel actual speed in cm/s
 */
float motor_get_right_speed(void);

#endif /* _HAL_MOTOR_H */