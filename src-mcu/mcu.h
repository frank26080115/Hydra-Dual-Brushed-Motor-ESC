#pragma once

#include "conf.h"
#include "defs.h"
#include "types.h"
#include "targets.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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
