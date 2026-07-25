/**
 * @file hal_relay.h
 * @brief GPIO relay driver (GPIO-controlled relay module)
 *
 * Controls a single relay via GPIO push-pull output.
 * Default pin: PB2 (unused in current pin map). Change RELAY_PORT / RELAY_PIN
 * below to match your hardware, or add a "RELAY" GPIO output in SysConfig and
 * uncomment the IOMUX-based init in hal_relay.c.
 */
#ifndef __HAL_RELAY_H__
#define __HAL_RELAY_H__

#include "ti_msp_dl_config.h"
#include <stdint.h>

/* ---- Auto-detected from SysConfig "relay" GPIO output ---- */
#ifndef RELAY_PORT
#define RELAY_PORT  relay_PORT        /* GPIOB (PB13, IOMUX_PINCM30) */
#endif
#ifndef RELAY_PIN
#define RELAY_PIN   relay_pin_0_PIN   /* DL_GPIO_PIN_13 */
#endif

void Relay_Init(void);
void Relay_On(void);
void Relay_Off(void);
void Relay_Toggle(void);
uint8_t Relay_GetState(void);

#endif /* __HAL_RELAY_H__ */
