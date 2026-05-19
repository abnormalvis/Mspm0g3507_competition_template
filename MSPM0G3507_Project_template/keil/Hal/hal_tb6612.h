#ifndef _HAL_TB6612_H
#define _HAL_TB6612_H

#include "stdint.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_gpio.h>
void Motor_Foreward_Left(uint16_t PWM_Hight);
void Motor_Backward_Left(uint16_t PWM_Hight);
void Motor_Foreward_Right(uint16_t PWM_Hight);
void Motor_Backward_Right(uint16_t PWM_Hight);
void Motor_Stop_All(void);
void Set_Pwm(int a, int b, int c, int d);

/* Debug: force direct PWM output, bypass PID */
void Set_Pwm_Debug(int left_duty, int right_duty);





#define AIN1_OUT(X)  ( (X) ? (DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_25)) : (DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_25)) )
#define AIN2_OUT(X)  ( (X) ? (DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_26)) : (DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_26)) )

#define BIN1_OUT(X)  ( (X) ? (DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_24)) : (DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_24)) )
#define BIN2_OUT(X)  ( (X) ? (DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_22)) : (DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_22)) )


#endif

/* 参考
#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "headfile.h"
//驱动频率20kHZ，占空比设置范围10到100
// 定义PWM范围
#define PWM_MAX 100
#define PWM_MIN -100
// PWM通道定义
#define MOTOR_L_PWM_CHANNEL TIM_CHANNEL_2 // 左轮PWM通道
#define MOTOR_R_PWM_CHANNEL TIM_CHANNEL_3 // 右轮PWM通道
// 电机转向控制引脚定义
#define MOTOR_L_IN1_PIN GPIO_PIN_12 // PB12
#define MOTOR_L_IN2_PIN GPIO_PIN_4 // PB4
#define MOTOR_R_IN1_PIN GPIO_PIN_1  // PB1
#define MOTOR_R_IN2_PIN GPIO_PIN_5 // PB5
#define MOTOR_GPIO_PORT GPIOB
extern int8_t motorL_dir;
extern int8_t motorR_dir;
void Limit(int *motor_speed);
void Set_Speed(int motor_l ,int motor_r);
void Set_MotorL_Speed(int motor_l);
void Set_MotorR_Speed(int motor_r);
#endif
 */
