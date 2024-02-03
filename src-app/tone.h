#pragma once

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DISABLE_LED

void tone_task(void);

// freq_multi: range 1 or more, 1 means 500 Hz tone, 2 means 1000 Hz, 3 means 1500 Hz, 40 is 20 kHz
// duration: in milliseconds, 0 means forever
// volume: range 0 to 100
void tone_start(uint8_t freq_multi, uint32_t duration, uint8_t volume);

void tone_stop(void);

#else

#define tone_task(...)
#define tone_start(...)
#define tone_stop(...)

#endif

#ifdef __cplusplus
}
#endif
