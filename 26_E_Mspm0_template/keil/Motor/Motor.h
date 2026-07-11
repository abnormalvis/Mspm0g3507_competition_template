#ifndef __MOTOR_H__
#define __MOTOR_H__

/* Motor driver selection - uncomment exactly ONE */
#define MOTOR_DRIVER_C107A    /* TB6612 with direction GPIO pins (default) */
// #define MOTOR_DRIVER_AT8236  /* AT8236 complementary PWM, no direction GPIO */

void Motor_SetPWML(float Speed);
void Motor_SetPWMR(float Speed);
void Motor_Init(void);

#endif
