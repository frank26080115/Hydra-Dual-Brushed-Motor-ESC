#pragma once

#ifdef STMICRO
#include "stm32.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

void mcu_init(void);

#ifdef __cplusplus
}
#endif
