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





#define AIN1_OUT(X)  ( (X) ? (DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_25)) : (DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_25)) )
#define AIN2_OUT(X)  ( (X) ? (DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_26)) : (DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_26)) )

#define BIN1_OUT(X)  ( (X) ? (DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_24)) : (DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_24)) )
#define BIN2_OUT(X)  ( (X) ? (DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_22)) : (DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_22)) )


#endif

