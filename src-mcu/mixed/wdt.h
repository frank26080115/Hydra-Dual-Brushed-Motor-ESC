#pragma once

#include "mcu.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
this is the header file for the watchdog timer, which should reset the entire ESC if the code has frozen for unknown reasons
this is intended to catch extraordinary events, like... solar radiation hitting the RAM causing a crash
this is not intended to be a crutch for ordinary bugs and bad coding
the watchdog timer will only be active once the ESC is in the main loop
*/

void wdt_init(void);
void wdt_feed_fromMain(bool);
void wdt_feed_onPulse(void);

#ifdef __cplusplus
}
#endif
