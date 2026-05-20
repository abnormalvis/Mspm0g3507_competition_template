#ifndef _HAL_TB6612_H
#define _HAL_TB6612_H

#include "stdint.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_gpio.h>

#define AIN1_OUT(X) ((X) ? (DL_GPIO_setPins(Motor_diection_PORT, Motor_diection_AIN1_PIN)) : (DL_GPIO_clearPins(Motor_diection_PORT, Motor_diection_AIN1_PIN)))
#define AIN2_OUT(X) ((X) ? (DL_GPIO_setPins(Motor_diection_PORT, Motor_diection_AIN2_PIN)) : (DL_GPIO_clearPins(Motor_diection_PORT, Motor_diection_AIN2_PIN)))
#define BIN1_OUT(X) ((X) ? (DL_GPIO_setPins(Motor_diection_PORT, Motor_diection_BIN1_PIN)) : (DL_GPIO_clearPins(Motor_diection_PORT, Motor_diection_BIN1_PIN)))
#define BIN2_OUT(X) ((X) ? (DL_GPIO_setPins(Motor_diection_PORT, Motor_diection_BIN2_PIN)) : (DL_GPIO_clearPins(Motor_diection_PORT, Motor_diection_BIN2_PIN)))

void Motor_Stop(void);
void Set_MotorL_Speed(uint8_t dir, uint32_t speed);
void Set_MotorR_Speed(uint8_t dir, uint32_t speed);

#endif
