#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void eeprom_write(uint8_t* buff, int len, uint32_t addr);

#ifdef __cplusplus
}
#endif
