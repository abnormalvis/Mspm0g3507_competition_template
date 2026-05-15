#include "ti/driverlib/dl_gpio.h"
#include "hal_led.h"

/* Usage: Turn on all RGB LEDs
 * Input params: None
 * Return value: None
*/
void RGB_ALL_open(void)
{
	DL_GPIO_setPins(RGB_GPIO, RGB_Red_Pin);       
	DL_GPIO_setPins(RGB_GPIO, RGB_Green_Pin); 
	DL_GPIO_setPins(RGB_GPIO, RGB_Blue_Pin); 	
}

/* Usage: Turn off all RGB LEDs
 * Input params: None
 * Return value: None
*/
void RGB_ALL_close(void)
{
	DL_GPIO_clearPins(RGB_GPIO, RGB_Red_Pin);       
	DL_GPIO_clearPins(RGB_GPIO, RGB_Green_Pin); 
	DL_GPIO_clearPins(RGB_GPIO, RGB_Blue_Pin); 	
}

/* Usage: Turn on red LED
 * Input params: None
 * Return value: None
*/
void RGB_Red_open(void)
{
	DL_GPIO_setPins(RGB_GPIO, RGB_Red_Pin);       
}

/* Usage: Turn on blue LED
 * Input params: None
 * Return value: None
*/
void RGB_Blue_open(void)
{
	DL_GPIO_setPins(RGB_GPIO, RGB_Blue_Pin);        
}

/* Usage: Turn on green LED
 * Input params: None
 * Return value: None
*/
void RGB_Green_open(void)
{
	DL_GPIO_setPins(RGB_GPIO, RGB_Green_Pin);         
}

/* Usage: Turn off green LED
 * Input params: None
 * Return value: None
*/
void RGB_Green_close(void)
{
	DL_GPIO_clearPins(RGB_GPIO, RGB_Green_Pin);         
}

/* Usage: Turn off red LED
 * Input params: None
 * Return value: None
*/
void RGB_Red_close(void)
{
	DL_GPIO_clearPins(RGB_GPIO, RGB_Red_Pin);         
}

/* Usage: Turn off blue LED
 * Input params: None
 * Return value: None
*/
void RGB_Blue_close(void)
{
	DL_GPIO_clearPins(RGB_GPIO, RGB_Blue_Pin);         
}







