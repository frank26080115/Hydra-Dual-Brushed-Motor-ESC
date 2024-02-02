#pragma once

#include "mcu.h"
#include "stm32_at32_compat.h"

#ifdef __cplusplus
extern "C" {
#endif

void swdio_init(uint32_t pull);
void swclk_init(uint32_t pull);

void swdpins_deinit(void);          // return to being used for debugging

static inline bool swclk_read(void)
{
    return LL_GPIO_IsInputPinSet(GPIOA, GPIO_PIN_SWCLK);
}

static inline bool swdio_read(void)
{
    return LL_GPIO_IsInputPinSet(GPIOA, GPIO_PIN_SWDIO);
}

#ifdef __cplusplus
}
#endif
