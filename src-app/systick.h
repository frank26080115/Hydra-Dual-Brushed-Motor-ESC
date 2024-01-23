#pragma once

#include "mcu.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern volatile uint32_t systick_cnt;

void systick_init(void);

#if defined(STMICRO)
#define millis() systick_cnt
#else
uint32_t millis(void);
#endif

#ifdef __cplusplus
}
#endif
