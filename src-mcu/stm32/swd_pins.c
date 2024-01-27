#include "swd_pins.h"

void swdpins_init(uint32_t pull)
{
    if (LL_GPIO_GetPinMode(GPIOA, GPIO_PIN_SWDIO) != LL_GPIO_MODE_INPUT || LL_GPIO_GetPinMode(GPIOA, GPIO_PIN_SWCLK) != LL_GPIO_MODE_INPUT || LL_GPIO_GetPinPull(GPIOA, GPIO_PIN_SWDIO) != pull || LL_GPIO_GetPinPull(GPIOA, GPIO_PIN_SWCLK) != pull)
    {
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin        = GPIO_PIN_SWDIO | GPIO_PIN_SWCLK;
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_INPUT;
        GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull       = pull;
        GPIO_InitStruct.Alternate  = 0;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void swdpins_deinit(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin        = GPIO_PIN_SWDIO | GPIO_PIN_SWCLK;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
