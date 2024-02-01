#include "swd_pins.h"

void swclk_init(uint32_t pull)
{
    gpio_mode_QUICK(GPIOA, GPIO_MODE_INPUT, pull, GPIO_PIN_SWCLK);
}

void swdio_init(uint32_t pull)
{
    gpio_mode_QUICK(GPIOA, GPIO_MODE_INPUT, pull, GPIO_PIN_SWDIO);
}

void swdpins_deinit(void)
{
    gpio_mode_QUICK(GPIOA, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PIN_SWCLK);
    gpio_mode_QUICK(GPIOA, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PIN_SWDIO);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE14, GPIO_MUX_0)
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE13, GPIO_MUX_0)
}
