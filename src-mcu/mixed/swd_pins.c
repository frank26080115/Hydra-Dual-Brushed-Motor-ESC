#include "swd_pins.h"

void swclk_init(uint32_t pull)
{
    #if defined(STMICRO)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin        = GPIO_PIN_SWCLK;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = pull;
    GPIO_InitStruct.Alternate  = 0;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    #elif defined(ARTERY)
    gpio_mode_QUICK(GPIOA, GPIO_MODE_INPUT, pull, GPIO_PIN_SWCLK);
    #endif
}

void swdio_init(uint32_t pull)
{
    #if defined(STMICRO)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin        = GPIO_PIN_SWDIO;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = pull;
    GPIO_InitStruct.Alternate  = 0;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    #elif defined(ARTERY)
    gpio_mode_QUICK(GPIOA, GPIO_MODE_INPUT, pull, GPIO_PIN_SWDIO);
    #endif
}

void swdpins_deinit(void)
{
    #if defined(STMICRO)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin        = GPIO_PIN_SWDIO | GPIO_PIN_SWCLK;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    #elif defined(ARTERY)
    gpio_mode_QUICK(GPIOA, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PIN_SWCLK);
    gpio_mode_QUICK(GPIOA, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PIN_SWDIO);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE14, GPIO_MUX_0);
    gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE13, GPIO_MUX_0);
    #endif
}
