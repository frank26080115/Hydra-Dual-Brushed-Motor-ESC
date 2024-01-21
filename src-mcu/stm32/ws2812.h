#pragma once

#include "mcu.h"
#include "led.h"

#ifdef __cplusplus
extern "C" {
#endif

void WS2812_init(void);
void WS2812_setRGB(uint8_t red, uint8_t green, uint8_t blue);

#ifdef __cplusplus
}
#endif
