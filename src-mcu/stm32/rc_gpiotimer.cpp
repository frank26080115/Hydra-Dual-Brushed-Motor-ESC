#include "rc.h"
#include "gpio_to_exti.h"
#include "stm32_at32_compat.h"
#include "rc_gpiotimer_shared.h"

void RcPulse_GpioIsr::init(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin)
{
    _tim  = TIMx;
    _gpio = GPIOx;
    _pin  = pin;

    rc_tim  = _tim;
    rc_gpio = _gpio;
    rc_pin  = _pin;

    rc_exti_line    = gpio_to_exti_line    (rc_pin);
    rc_exti_port    = gpio_to_exti_port    (rc_pin);
    rc_exti_sysline = gpio_to_exti_sys_line(rc_pin);

    LL_TIM_InitTypeDef timcfg = {0};
    timcfg.Prescaler     = __LL_TIM_CALC_PSC(SystemCoreClock, 1000000 * RC_MEASURE_MULTIPLIER);
    timcfg.CounterMode   = LL_TIM_COUNTERMODE_UP;
    timcfg.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    timcfg.Autoreload    = -1; // this will make it interrupt at overflow event
    LL_TIM_Init(rc_tim, &timcfg);
    // every count is 0.25 us , this works for both 8 MHz and 48 MHz system core clock

    LL_TIM_EnableIT_UPDATE(rc_tim);
    LL_TIM_EnableCounter(rc_tim);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin          = rc_pin;
    GPIO_InitStruct.Mode         = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed        = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType   = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull         = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(rc_gpio, &GPIO_InitStruct);

    #if defined(MCU_F051)
    LL_SYSCFG_SetEXTISource(rc_exti_port, rc_exti_sysline);
    #elif defined(MCU_G071)
    LL_EXTI_SetEXTISource(rc_exti_port, rc_exti_sysline);
    #else
    #error
    #endif
    LL_EXTI_EnableRisingTrig_0_31 (rc_exti_line);
    LL_EXTI_EnableFallingTrig_0_31(rc_exti_line);
    LL_EXTI_EnableEvent_0_31      (rc_exti_line);
    LL_EXTI_EnableIT_0_31         (rc_exti_line);

    NVIC_SetPriority(GPIOEXTI_TIM_IRQn, 1);
    NVIC_EnableIRQ  (GPIOEXTI_TIM_IRQn);
    NVIC_SetPriority(GPIOEXTI_IRQn, 0);
    NVIC_EnableIRQ  (GPIOEXTI_IRQn);

    was_high = LL_GPIO_IsInputPinSet(rc_gpio, rc_pin);

    rc_find_arming_vals(RC_MEASURE_MULTIPLIER, GPIO_RC_PULSE_OFFSET, (uint16_t*)&arming_val_min, (uint16_t*)&arming_val_max);
}
