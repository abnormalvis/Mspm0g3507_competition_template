#ifndef _HAL_LED_H
#define _HAL_LED_H

#define RGB_GPIO			GPIOB
#define RGB_Red_Pin		DL_GPIO_PIN_26
#define RGB_Blue_Pin	DL_GPIO_PIN_22
#define RGB_Green_Pin	DL_GPIO_PIN_27
#define RGB_ALL_Pin   RGB_Blue_Pin|RGB_Red_Pin|RGB_Green_Pin

void RGB_ALL_open(void);
void RGB_ALL_close(void);
void RGB_Red_open(void);
void RGB_Blue_open(void);
void RGB_Green_open(void);
void RGB_Green_close(void);
void RGB_Red_close(void);
void RGB_Blue_close(void);

#endif

