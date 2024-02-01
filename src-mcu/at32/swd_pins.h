#pragma once

#include "mcu.h"

#ifdef __cplusplus
extern "C" {
#endif

void swdio_init(uint32_t pull);
void swclk_init(uint32_t pull);

void swdpins_deinit(void);          // return to being used for debugging

static inline bool swclk_read(void)
{
    return gpio_input_data_bit_read(GPIOA, GPIO_PIN_SWCLK);
}

static inline bool swdio_read(void)
{
    return gpio_input_data_bit_read(GPIOA, GPIO_PIN_SWDIO);
}

#ifdef __cplusplus
}
#endif
