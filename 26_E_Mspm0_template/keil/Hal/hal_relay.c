/**
 * @file hal_relay.c
 * @brief GPIO relay driver implementation
 *
 * Controls a general-purpose relay module via GPIO push-pull output.
 * The relay defaults to OFF (low) after init.
 *
 * If you add a "RELAY" GPIO output pin in SysConfig, the generated
 * SYSCFG_DL_init() will configure the IOMUX automatically. In that case
 * the manual DOE register write in Relay_Init() is redundant but harmless.
 */
#include "hal_relay.h"

static uint8_t s_relay_state = 0;   /* 0 = off, 1 = on */

void Relay_Init(void)
{
    /* Set pin low (relay off) before enabling output */
    DL_GPIO_clearPins(RELAY_PORT, RELAY_PIN);

    /* Enable output direction (manual register write — works even without
     * SysConfig IOMUX entry). If the pin IS configured in SysConfig, DOE
     * is already set and this is a no-op. */
    RELAY_PORT->DOE31_0 |= RELAY_PIN;

    s_relay_state = 0;
}

void Relay_On(void)
{
    DL_GPIO_setPins(RELAY_PORT, RELAY_PIN);
    s_relay_state = 1;
}

void Relay_Off(void)
{
    DL_GPIO_clearPins(RELAY_PORT, RELAY_PIN);
    s_relay_state = 0;
}

void Relay_Toggle(void)
{
    if (s_relay_state) {
        Relay_Off();
    } else {
        Relay_On();
    }
}

uint8_t Relay_GetState(void)
{
    return s_relay_state;
}
