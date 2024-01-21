#pragma once

#include "conf.h"
#include "defs.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((section(".firmware_info")))
extern const firmware_info_s firmware_info;

#define ENSURE_VERSION_DATA_IS_KEPT()    do { volatile uint8_t* useless = (volatile uint8_t*)&firmware_info; volatile uint8_t useless2 = useless[0]; volatile uint8_t useless3 = useless[0]; if (useless2 == useless3) { __NOP(); } } while (0)

#ifdef __cplusplus
}
#endif
