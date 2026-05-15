#ifndef _HAL_KEY_H
#define _HAL_KEY_H

#include <stdint.h>

//void delay_ms(unsigned int ms);
//uint8_t Key_GetNum(void);


// Up key C5
#define K1_PORT	GPIOB
#define K1_PIN	DL_GPIO_PIN_13


// Down key C7
#define K2_PORT	GPIOB
#define K2_PIN	DL_GPIO_PIN_19

// Left key
#define K3_PORT	GPIOB
#define K3_PIN	DL_GPIO_PIN_23

// Right key C6
#define K4_PORT	GPIOB
#define K4_PIN	DL_GPIO_PIN_18
 

// Center key C9
#define K5_PORT GPIOB	
#define K5_PIN	DL_GPIO_PIN_24

typedef enum
{
	KEY_S1,		// Up
	KEY_S2,		// Down
	KEY_S3,		// Left
	KEY_S4,		// Right
	KEY_S5,		// Cancel / Center
	//KEY_S6,		// Confirm / Menu
	KEYNUM
}KEY_TYPEDEF;			// Key count

 

// Key scan process
typedef enum
{
	KEY_STEP_WAIT,			// Wait for press
	KEY_STEP_CLICK,				// Key debounce
	KEY_STEP_LONG_PRESS,				// Long press
	KEY_STEP_CONTINUOUS_PRESS,			// Continuous press
}KEY_STEP_TYPEDEF;




typedef enum
{	
	KEY_IDLE,       	 							// No key event
	KEY_CLICK,          								// Click confirm
	KEY_CLICK_RELEASE,            			// Click release
	KEY_LONG_PRESS,			   					 	// Long press confirm
	KEY_LONG_PRESS_CONTINUOUS,						// Continuous press
	KEY_LONG_PRESS_RELEASE								// Long press release
	 
}KEY_EVENT_TYPEDEF;

typedef enum
{
	KEY_IDLE_VAL,	// 0: None
	KEY1_CLICK,		// 1
	KEY1_CLICK_RELEASE,		// 2
	KEY1_LONG_PRESS,			// 3
	KEY1_LONG_PRESS_CONTINUOUS,	// 4
	KEY1_LONG_PRESS_RELEASE,		// 5
	
	KEY2_CLICK,								// 6: Key 2
	KEY2_CLICK_RELEASE,
	KEY2_LONG_PRESS,
	KEY2_LONG_PRESS_CONTINUOUS,
	KEY2_LONG_PRESS_RELEASE,
	
	KEY3_CLICK,							// 11: Key 3
	KEY3_CLICK_RELEASE,
	KEY3_LONG_PRESS,
	KEY3_LONG_PRESS_CONTINUOUS,
	KEY3_LONG_PRESS_RELEASE,
	
	KEY4_CLICK,						// 16: Key 4
	KEY4_CLICK_RELEASE,
	KEY4_LONG_PRESS,
	KEY4_LONG_PRESS_CONTINUOUS,
	KEY4_LONG_PRESS_RELEASE,
	
	KEY5_CLICK,						// 21: Key 5
	KEY5_CLICK_RELEASE,
	KEY5_LONG_PRESS,
	KEY5_LONG_PRESS_CONTINUOUS,
	KEY5_LONG_PRESS_RELEASE,

	
	
}KEY_VALUE_TYPEDEF;


typedef void (*KeyEvent_CallBack_t)(KEY_VALUE_TYPEDEF keys);


// Key scan interval, 10ms per tick, 2 = 20ms
	#define KEY_SCANTIME	2		// 20ms

// Key long press duration
	#define	KEY_PRESS_LONG_TIME	200	// 2s

// Key continuous trigger interval
	#define KEY_PRESS_CONTINUE_TIME	15	// 150ms

void hal_KeyProc(void);
void hal_KeyScanCBSRegister(KeyEvent_CallBack_t pCBS);
void hal_KeyInit(void);

#endif

