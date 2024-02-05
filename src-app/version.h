#pragma once

#include "conf.h"
#include "defs.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((section(".firmware_info")))
extern const firmware_info_s firmware_info;

#ifdef __cplusplus
}
#endif
