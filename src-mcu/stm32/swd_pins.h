#pragma once

#include "mcu.h"

#ifdef __cplusplus
extern "C" {
#endif

void swdpins_init(uint32_t pull);

static inline bool swclk_read(bool x)
{
    return LL_GPIO_IsInputPinSet(GPIOA, GPIO_PIN_SWCLK);
}

static inline bool swdio_read(bool x)
{
    return LL_GPIO_IsInputPinSet(GPIOA, GPIO_PIN_SWDIO);
}

#ifdef __cplusplus
}
#endif
