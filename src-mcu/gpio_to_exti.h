#pragma once
#include "mcu.h"

// TODO: add more items here for wider support

static inline uint32_t gpio_to_exti_line(uint32_t x)
{
    switch (x)
    {
        #if defined(MCU_F051) || defined(MCU_G071)
        case LL_GPIO_PIN_6 : return LL_EXTI_LINE_6;
        case LL_GPIO_PIN_13: return LL_EXTI_LINE_13;
        case LL_GPIO_PIN_14: return LL_EXTI_LINE_14;
        #elif defined(MCU_F421)
        case GPIO_PINS_6 : return EXINT_LINE_6;
        case GPIO_PINS_13: return EXINT_LINE_13;
        case GPIO_PINS_14: return EXINT_LINE_14;
        #endif
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
        #if defined(MCU_F421)
        case GPIO_PINS_6 : return SCFG_PINS_SOURCE6;
        case GPIO_PINS_13: return SCFG_PINS_SOURCE13;
        case GPIO_PINS_14: return SCFG_PINS_SOURCE14;
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
        #if defined(MCU_F421)
        case GPIO_PINS_6 : return SCFG_PORT_SOURCE_GPIOB;
        case GPIO_PINS_13: return SCFG_PORT_SOURCE_GPIOA;
        case GPIO_PINS_14: return SCFG_PORT_SOURCE_GPIOA;
        #endif
    }
    return 0;
}
