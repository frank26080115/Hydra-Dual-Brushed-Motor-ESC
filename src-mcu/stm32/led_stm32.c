#include "led.h"
#ifdef USE_LED_STRIP
#include "ws2812.h"
#endif

void led_init_gpio(GPIO_TypeDef *GPIOx, uint32_t Pin, bool opendrain)
{
#ifndef DISABLE_LED
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin        = Pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = opendrain ? LL_GPIO_OUTPUT_OPENDRAIN : LL_GPIO_OUTPUT_PUSHPULL; // TODO: is there any benefit to not using push-pull all the time? switch back if issues arise
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOx, &GPIO_InitStruct);
    if (opendrain) {
        LL_GPIO_SetOutputPin(GPIOx, Pin);
    }
    else {
        LL_GPIO_ResetOutputPin(GPIOx, Pin);
    }
#endif
}

void led_set(GPIO_TypeDef* GPIOx, uint32_t Pin, bool ison, bool opendrain)
{
#ifndef DISABLE_LED
    if ((ison && opendrain) || (!ison && !opendrain)) {
        LL_GPIO_ResetOutputPin(GPIOx, Pin);
    }
    else {
        LL_GPIO_SetOutputPin(GPIOx, Pin);
    }
#endif
}

void ledhw_init(void)
{
#ifndef DISABLE_LED
#ifdef USE_LED_STRIP
    WS2812_init();
#endif
#ifdef USE_RGB_LED
    led_init_gpio(LED_GPIO_RED  , LED_PIN_RED  , LED_IS_OPENDRAIN);
    #ifdef LED_GPIO_RED2
    led_init_gpio(LED_GPIO_RED2 , LED_PIN_RED2 , LED_IS_OPENDRAIN);
    #endif
    led_init_gpio(LED_GPIO_GREEN, LED_PIN_GREEN, LED_IS_OPENDRAIN);
    led_init_gpio(LED_GPIO_BLUE , LED_PIN_BLUE , LED_IS_OPENDRAIN);
#elif defined(LED_GPIO) && defined(LED_PIN)
    led_init_gpio(LED_GPIO, LED_PIN, LED_IS_OPENDRAIN);
#endif
#endif
}
