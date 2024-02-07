#include "led.h"
#ifdef USE_LED_STRIP
#include "ws2812.h"
#endif
#include "stm32_at32_compat.h"

void led_init_gpio(GPIO_TypeDef *GPIOx, uint32_t Pin, bool opendrain)
{
#ifndef DISABLE_LED
    #if defined(STMICRO)
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
    #elif defined(ARTERY)
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_pins = Pin;
    gpio_init_struct.gpio_out_type = opendrain ? GPIO_OUTPUT_OPEN_DRAIN : GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

    gpio_init(GPIOx, &gpio_init_struct);

    if (opendrain) {
        gpio_bits_set(GPIOx, Pin);
    }
    else {
        gpio_bits_reset(GPIOx, Pin);
    }
    #endif
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
    #ifdef LED_PIN_BLUE2
    led_init_gpio(LED_GPIO_BLUE2, LED_PIN_BLUE2, LED_IS_OPENDRAIN);
    #endif
#elif defined(LED_GPIO) && defined(LED_PIN)
    led_init_gpio(LED_GPIO, LED_PIN, LED_IS_OPENDRAIN);
#endif
#endif
}
