/**
 * @file hal_relay.c
 * @brief CANIO relay driver implementation
 *
 * Controls a single relay (electromagnet) via the CANIO protocol over CANFD0.
 * The relay hardware is connected to a 众盛科技 ZS-DIO CANIO module.
 *
 * Because CANIO function 0x01 overwrites ALL 48 channels, a local shadow
 * buffer tracks the full channel state. Each Relay_On/Off modifies the
 * target bit in the shadow and sends the complete 8-byte frame.
 *
 * The old GPIO-based implementation used PB13 (DL_GPIO_setPins/clearPins).
 * That pin can now be repurposed or left unconnected.
 */
#include "hal_relay.h"

static uint8_t s_relay_state  = 0;   /* 0 = off, 1 = on */
static uint8_t s_relay_shadow[8];    /* full 48-channel shadow buffer */

void Relay_Init(void)
{
    uint8_t i;

    /* Clear shadow buffer: all channels off */
    for (i = 0; i < 8U; i++) {
        s_relay_shadow[i] = 0x00U;
    }

    /* Initialize CANIO and send all-off frame */
    CANIO_Init();

    s_relay_state = 0;
}

void Relay_On(void)
{
    /* Set channel bit in shadow and send full frame */
    CANIO_SetRelayCh(s_relay_shadow, CANIO_RELAY_CH, 1U);
    CANIO_WriteRelay(s_relay_shadow);

    s_relay_state = 1;
}

void Relay_Off(void)
{
    /* Clear channel bit in shadow and send full frame */
    CANIO_SetRelayCh(s_relay_shadow, CANIO_RELAY_CH, 0U);
    CANIO_WriteRelay(s_relay_shadow);

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
