#include "ti/driverlib/dl_gpio.h"
#include "hal_led.h"

/* 作用 ： 打开RGB所有灯
 * 传入参数 ：无
 * 返回值 ： 无
*/
void RGB_ALL_open(void)
{
	DL_GPIO_setPins(RGB_GPIO, RGB_Red_Pin);       
	DL_GPIO_setPins(RGB_GPIO, RGB_Green_Pin); 
	DL_GPIO_setPins(RGB_GPIO, RGB_Blue_Pin); 	
}

/* 作用 ： 关闭RGB所有灯
 * 传入参数 ：无
 * 返回值 ： 无
*/
void RGB_ALL_close(void)
{
	DL_GPIO_clearPins(RGB_GPIO, RGB_Red_Pin);       
	DL_GPIO_clearPins(RGB_GPIO, RGB_Green_Pin); 
	DL_GPIO_clearPins(RGB_GPIO, RGB_Blue_Pin); 	
}

/* 作用 ： 打开红灯
 * 传入参数 ：无
 * 返回值 ： 无
*/
void RGB_Red_open(void)
{
	DL_GPIO_setPins(RGB_GPIO, RGB_Red_Pin);       
}

/* 作用 ： 打开蓝灯
 * 传入参数 ：无
 * 返回值 ： 无
*/
void RGB_Blue_open(void)
{
	DL_GPIO_setPins(RGB_GPIO, RGB_Blue_Pin);        
}

/* 作用 ： 打开绿灯
 * 传入参数 ：无
 * 返回值 ： 无
*/
void RGB_Green_open(void)
{
	DL_GPIO_setPins(RGB_GPIO, RGB_Green_Pin);         
}

/* 作用 ： 关闭绿灯
 * 传入参数 ：无
 * 返回值 ： 无
*/
void RGB_Green_close(void)
{
	DL_GPIO_clearPins(RGB_GPIO, RGB_Green_Pin);         
}

/* 作用 ： 关闭红灯
 * 传入参数 ：无
 * 返回值 ： 无
*/
void RGB_Red_close(void)
{
	DL_GPIO_clearPins(RGB_GPIO, RGB_Red_Pin);         
}

/* 作用 ： 关闭蓝灯
 * 传入参数 ：无
 * 返回值 ： 无
*/
void RGB_Blue_close(void)
{
	DL_GPIO_clearPins(RGB_GPIO, RGB_Blue_Pin);         
}







