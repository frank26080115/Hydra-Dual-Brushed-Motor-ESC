#pragma once

#include "mcu.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint32_t systick_cnt;
#ifdef DISABLE_LED
extern volatile uint32_t tone_freq_multi;
#else
#define tone_freq_multi    (1)
#endif

void systick_init(void);

#if defined(STMICRO)
#define millis() (systick_cnt / tone_freq_multi)
#elif defined(ARTERY)
#define millis() (systick_cnt / tone_freq_multi)
#else
#error
uint32_t millis(void);
#endif

#ifdef __cplusplus
}
#endif
