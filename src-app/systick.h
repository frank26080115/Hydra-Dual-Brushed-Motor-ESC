#pragma once

#include "mcu.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void systick_init(void);

#if defined(STMICRO)
#define millis() SysTick->VAL
#else
uint32_t millis(void);
#endif

#ifdef __cplusplus
}
#endif
