#include "Laser.h"

/* Laser/LED GPIOs are not yet configured in SysConfig.
 * These functions are stubs until pins are added — calls preserved
 * so duty_chess.c and other callers compile. */

void Laser_1_on(void)  { /* TODO: add Laser_1 pin to syscfg */ }
void Laser_1_off(void) { /* TODO: add Laser_1 pin to syscfg */ }
void Laser_2_on(void)  { /* TODO: add Laser_2 pin to syscfg */ }
void Laser_2_off(void) { /* TODO: add Laser_2 pin to syscfg */ }
void LED_on(void)      { /* TODO: add LED pin to syscfg */ }
void LED_off(void)     { /* TODO: add LED pin to syscfg */ }
