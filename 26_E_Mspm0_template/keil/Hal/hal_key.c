#include "hal_key.h"
#include <ti/driverlib/dl_gpio.h>

static KeyEvent_CallBack_t KeyScanCBS;

static unsigned char hal_getKey1Sta(void) { return (!DL_GPIO_readPins(KEY_PORT, KEY_KEY_UP_PIN));    }
static unsigned char hal_getKey2Sta(void) { return (!DL_GPIO_readPins(KEY_PORT, KEY_KEY_DOWN_PIN));  }
static unsigned char hal_getKey3Sta(void) { return (!DL_GPIO_readPins(KEY_PORT, KEY_KEY_LEFT_PIN));  }
static unsigned char hal_getKey4Sta(void) { return (!DL_GPIO_readPins(KEY_PORT, KEY_KEY_RIGHT_PIN)); }
static unsigned char hal_getKey5Sta(void) { return (!DL_GPIO_readPins(KEY_PORT, KEY_KEY_MID_PIN));   }

static unsigned char (*getKeysState[KEYNUM])(void) = {
    hal_getKey1Sta, hal_getKey2Sta, hal_getKey3Sta, hal_getKey4Sta, hal_getKey5Sta
};

static unsigned char  KeyStep[KEYNUM];
static unsigned short KeyScanTime[KEYNUM];
static unsigned short KeyPressLongTimer[KEYNUM];
static unsigned short KeyContPressTimer[KEYNUM];
static volatile unsigned char key_irq_pending[KEYNUM];

void hal_KeyInit(void)
{
    unsigned char i;
    KeyScanCBS = 0;
    for (i = 0; i < KEYNUM; i++)
    {
        KeyStep[i]            = KEY_STEP_WAIT;
        KeyScanTime[i]        = KEY_SCANTIME;
        KeyPressLongTimer[i]  = KEY_PRESS_LONG_TIME;
        KeyContPressTimer[i]  = KEY_PRESS_CONTINUE_TIME;
        key_irq_pending[i]    = 0;
    }
}

void hal_KeyScanCBSRegister(KeyEvent_CallBack_t pCBS)
{
    if (KeyScanCBS == 0)
        KeyScanCBS = pCBS;
}

void hal_key_isr_notify(uint32_t pin_status)
{
    if (pin_status & KEY_KEY_UP_PIN)    key_irq_pending[KEY_S1] = 1;
    if (pin_status & KEY_KEY_DOWN_PIN)  key_irq_pending[KEY_S2] = 1;
    if (pin_status & KEY_KEY_LEFT_PIN)  key_irq_pending[KEY_S3] = 1;
    if (pin_status & KEY_KEY_RIGHT_PIN) key_irq_pending[KEY_S4] = 1;
    if (pin_status & KEY_KEY_MID_PIN)   key_irq_pending[KEY_S5] = 1;
}

void hal_KeyProc(void)
{
    unsigned char i, state, keys;
    for (i = 0; i < KEYNUM; i++)
    {
        if (!key_irq_pending[i] && KeyStep[i] == KEY_STEP_WAIT)
            continue;

        keys  = 0;
        state = getKeysState[i]();

        switch (KeyStep[i])
        {
            case KEY_STEP_WAIT:
                if (state) KeyStep[i] = KEY_STEP_CLICK;
                else       key_irq_pending[i] = 0;
                break;

            case KEY_STEP_CLICK:
                if (state)
                {
                    if (!(--KeyScanTime[i]))
                    {
                        KeyScanTime[i] = KEY_SCANTIME;
                        KeyStep[i]     = KEY_STEP_LONG_PRESS;
                        keys = (i * 5) + 1;
                    }
                }
                else
                {
                    KeyScanTime[i] = KEY_SCANTIME;
                    KeyStep[i]     = KEY_STEP_WAIT;
                    key_irq_pending[i] = 0;
                }
                break;

            case KEY_STEP_LONG_PRESS:
                if (state)
                {
                    if (!(--KeyPressLongTimer[i]))
                    {
                        KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
                        KeyStep[i]           = KEY_STEP_CONTINUOUS_PRESS;
                        keys = (i * 5) + 3;
                    }
                }
                else
                {
                    KeyPressLongTimer[i] = KEY_PRESS_LONG_TIME;
                    KeyStep[i]           = KEY_STEP_WAIT;
                    key_irq_pending[i]   = 0;
                    keys = (i * 5) + 2;
                }
                break;

            case KEY_STEP_CONTINUOUS_PRESS:
                if (state)
                {
                    if (!(--KeyContPressTimer[i]))
                    {
                        KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
                        keys = (i * 5) + 4;
                    }
                }
                else
                {
                    KeyContPressTimer[i] = KEY_PRESS_CONTINUE_TIME;
                    KeyStep[i]           = KEY_STEP_WAIT;
                    key_irq_pending[i]   = 0;
                    keys = (i * 5) + 5;
                }
                break;
        }

        if (keys && KeyScanCBS)
            KeyScanCBS((KEY_VALUE_TYPEDEF)keys);
    }
}
