#include "led.h"
#ifdef USE_LED_STRIP
#include "ws2812.h"
#endif

void led_init_gpio(gpio_type *GPIOx, uint32_t Pin, bool opendrain)
{
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
}

void led_set(gpio_type* GPIOx, uint32_t Pin, bool ison, bool opendrain)
{
    if ((ison && opendrain) || (!ison && !opendrain)) {
        gpio_bits_reset(GPIOx, Pin);
    }
    else {
        gpio_bits_set(GPIOx, Pin);
    }
}

void ledhw_init(void)
{
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
}
