#include "led.h"

static inline void led_init_gpio(GPIO_TypeDef *GPIOx, uint32_t Pin, bool opendrain)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    #if defined(GPIOA)
    if (GPIOx == GPIOA) {
        #ifdef LL_AHB1_GRP1_PERIPH_GPIOA
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
        #endif
        #ifdef LL_IOP_GRP1_PERIPH_GPIOA
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
        #endif
    }
    #endif
    #if defined(GPIOB)
    if (GPIOx == GPIOB) {
        #ifdef LL_AHB1_GRP1_PERIPH_GPIOB
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
        #endif
        #ifdef LL_IOP_GRP1_PERIPH_GPIOB
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
        #endif
    }
    #endif
    #if defined(GPIOC)
    if (GPIOx == GPIOC) {
        #ifdef LL_AHB1_GRP1_PERIPH_GPIOC
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
        #endif
        #ifdef LL_IOP_GRP1_PERIPH_GPIOC
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
        #endif
    }
    #endif
    #if defined(GPIOD)
    if (GPIOx == GPIOD) {
        #ifdef LL_AHB1_GRP1_PERIPH_GPIOD
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
        #endif
        #ifdef LL_IOP_GRP1_PERIPH_GPIOD
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD);
        #endif
    }
    #endif
    #if defined(GPIOE)
    if (GPIOx == GPIOE) {
        #ifdef LL_AHB1_GRP1_PERIPH_GPIOE
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
        #endif
        #ifdef LL_IOP_GRP1_PERIPH_GPIOE
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOE);
        #endif
    }
    #endif
    #if defined(GPIOF)
    if (GPIOx == GPIOF) {
        #ifdef LL_AHB1_GRP1_PERIPH_GPIOF
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
        #endif
        #ifdef LL_IOP_GRP1_PERIPH_GPIOF
        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOF);
        #endif
    }
    #endif

    // TODO: add more clock initialization when required

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
}

static inline void led_set(GPIO_TypeDef *GPIOx, uint32_t Pin, bool ison, bool opendrain)
{
    if ((ison && opendrain) || (!ison && !opendrain)) {
        LL_GPIO_ResetOutputPin(GPIOx, Pin);
    }
    else {
        LL_GPIO_SetOutputPin(GPIOx, Pin);
    }
}

void ledhw_init(void)
{
#ifdef USE_LED_STRIP
    WS2812_init();
#endif
#ifdef USE_RGB_LED
    led_init_gpio(LED_GPIO_RED  , LED_PIN_RED  , LED_IS_OPENDRAIN);
    led_init_gpio(LED_GPIO_GREEN, LED_PIN_GREEN, LED_IS_OPENDRAIN);
    led_init_gpio(LED_GPIO_BLUE , LED_PIN_BLUE , LED_IS_OPENDRAIN);
#elif defined(LED_GPIOx) && defined(LED_PIN)
    led_init_gpio(LED_GPIO, LED_PIN, LED_IS_OPENDRAIN);
#endif
}
