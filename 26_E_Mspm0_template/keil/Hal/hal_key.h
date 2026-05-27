#ifndef _HAL_KEY_H
#define _HAL_KEY_H

#include <stdint.h>
#include "ti_msp_dl_config.h"

/* Manual KEY pin defines — was in sysconfig KEY GPIO group (now removed) */
#ifndef KEY_PORT
#define KEY_PORT            GPIOB
#define KEY_KEY_UP_PIN      DL_GPIO_PIN_14
#define KEY_KEY_DOWN_PIN    DL_GPIO_PIN_13
#define KEY_KEY_LEFT_PIN    DL_GPIO_PIN_17
#define KEY_KEY_RIGHT_PIN   DL_GPIO_PIN_19
#define KEY_KEY_MID_PIN     DL_GPIO_PIN_20
#endif

typedef enum
{
    KEY_S1,         // Up
    KEY_S2,         // Down
    KEY_S3,         // Left
    KEY_S4,         // Right
    KEY_S5,         // Center
    KEYNUM
} KEY_TYPEDEF;

typedef enum
{
    KEY_STEP_WAIT,
    KEY_STEP_CLICK,
    KEY_STEP_LONG_PRESS,
    KEY_STEP_CONTINUOUS_PRESS,
} KEY_STEP_TYPEDEF;

typedef enum
{
    KEY_IDLE_VAL,
    KEY1_CLICK,
    KEY1_CLICK_RELEASE,
    KEY1_LONG_PRESS,
    KEY1_LONG_PRESS_CONTINUOUS,
    KEY1_LONG_PRESS_RELEASE,

    KEY2_CLICK,
    KEY2_CLICK_RELEASE,
    KEY2_LONG_PRESS,
    KEY2_LONG_PRESS_CONTINUOUS,
    KEY2_LONG_PRESS_RELEASE,

    KEY3_CLICK,
    KEY3_CLICK_RELEASE,
    KEY3_LONG_PRESS,
    KEY3_LONG_PRESS_CONTINUOUS,
    KEY3_LONG_PRESS_RELEASE,

    KEY4_CLICK,
    KEY4_CLICK_RELEASE,
    KEY4_LONG_PRESS,
    KEY4_LONG_PRESS_CONTINUOUS,
    KEY4_LONG_PRESS_RELEASE,

    KEY5_CLICK,
    KEY5_CLICK_RELEASE,
    KEY5_LONG_PRESS,
    KEY5_LONG_PRESS_CONTINUOUS,
    KEY5_LONG_PRESS_RELEASE,
} KEY_VALUE_TYPEDEF;

typedef void (*KeyEvent_CallBack_t)(KEY_VALUE_TYPEDEF keys);

#define KEY_SCANTIME            2       // debounce ticks (10ms tick * 2 = 20ms)
#define KEY_PRESS_LONG_TIME     200     // 2s
#define KEY_PRESS_CONTINUE_TIME 15      // 150ms

void hal_KeyInit(void);
void hal_KeyScanCBSRegister(KeyEvent_CallBack_t pCBS);
void hal_KeyProc(void);
void hal_key_isr_notify(uint32_t pin_status);

#endif
