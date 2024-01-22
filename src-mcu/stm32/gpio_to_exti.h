#pragma once
#include "mcu.h"

static inline uint32_t gpio_to_exti_line(uint32_t x)
{
    switch (x)
    {
        case LL_GPIO_PIN_6 : return LL_EXTI_LINE_6;
        case LL_GPIO_PIN_13: return LL_EXTI_LINE_13;
        case LL_GPIO_PIN_14: return LL_EXTI_LINE_14;
    }
    return 0;
}

static inline uint32_t gpio_to_exti_sys_line(uint32_t x)
{
    switch (x)
    {
        #if defined(MCU_F051)
        case LL_GPIO_PIN_6 : return LL_SYSCFG_EXTI_LINE6;
        case LL_GPIO_PIN_13: return LL_SYSCFG_EXTI_LINE13;
        case LL_GPIO_PIN_14: return LL_SYSCFG_EXTI_LINE14;
        #endif
        #if defined(MCU_G071)
        case LL_GPIO_PIN_6 : return LL_EXTI_CONFIG_LINE6;
        case LL_GPIO_PIN_13: return LL_EXTI_CONFIG_LINE13;
        case LL_GPIO_PIN_14: return LL_EXTI_CONFIG_LINE14;
        #endif
    }
    return 0;
}

static inline uint32_t gpio_to_exti_port(uint32_t x)
{
    switch (x)
    {
        #if defined(MCU_F051)
        case LL_GPIO_PIN_6 : return LL_SYSCFG_EXTI_PORTB;
        case LL_GPIO_PIN_13: return LL_SYSCFG_EXTI_PORTA;
        case LL_GPIO_PIN_14: return LL_SYSCFG_EXTI_PORTA;
        #endif
        #if defined(MCU_G071)
        case LL_GPIO_PIN_6 : return LL_EXTI_CONFIG_PORTB;
        case LL_GPIO_PIN_13: return LL_EXTI_CONFIG_PORTA;
        case LL_GPIO_PIN_14: return LL_EXTI_CONFIG_PORTA;
        #endif
    }
    return 0;
}
