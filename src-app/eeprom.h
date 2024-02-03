#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "conf.h"
#include "defs.h"

#include <stdint.h>

void eeprom_write(uint32_t* buff, int len, uint32_t addr);

#ifdef __cplusplus
}
#endif
