#include "rc.h"
#include "rc_at32.h"
#include "gpio_to_exti.h"
#include "stm32_at32_compat.h"
#include "rc_gpiotimer_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef USE_LED_STRIP
void WS2812_startTmr6Anyways(void)
{
    tmr_clock_source_div_set(TMR6, TMR_CLOCK_DIV1);
    TMR6->div = (SystemCoreClock / (1000000 * RC_MEASURE_MULTIPLIER)) - 1; // 0.25us resolution
    TMR6->pr = -1;
    tmr_interrupt_enable(TMR6, TMR_OVF_INT, TRUE);
    tmr_counter_enable(TMR6, TRUE);
    NVIC_SetPriority(TMR6_GLOBAL_IRQn, 1);
    NVIC_EnableIRQ  (TMR6_GLOBAL_IRQn);
}
#endif

#ifdef __cplusplus
}
#endif

void RcPulse_GpioIsr::init(tmr_type* TIMx, gpio_type* GPIOx, uint32_t pin)
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

    exint_init_type cfg;
    exint_default_para_init(&cfg);
    cfg.line_mode     = EXINT_LINE_INTERRUPUT;
    cfg.line_select   = rc_exti_line;
    cfg.line_polarity = EXINT_TRIGGER_BOTH_EDGE;
    cfg.line_enable   = FALSE;
    exint_init(&cfg);
    scfg_exint_line_config((scfg_port_source_type)rc_exti_port, (scfg_pins_source_type)rc_exti_sysline);

    tmr_clock_source_div_set(rc_tim, TMR_CLOCK_DIV1);
    rc_tim->div = (SystemCoreClock / (1000000 * RC_MEASURE_MULTIPLIER)) - 1; // 0.25us resolution
    rc_tim->pr = -1;
    tmr_interrupt_enable(rc_tim, TMR_OVF_INT, TRUE);
    tmr_counter_enable(rc_tim, TRUE);

    gpio_init_type GPIO_InitStruct = { 0 };
    GPIO_InitStruct.gpio_mode           = GPIO_MODE_INPUT;
    GPIO_InitStruct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    GPIO_InitStruct.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
    GPIO_InitStruct.gpio_pull           = GPIO_PULL_DOWN;
    GPIO_InitStruct.gpio_pins           = rc_pin;
    gpio_init(rc_gpio, &GPIO_InitStruct);

    NVIC_SetPriority(TMR6_GLOBAL_IRQn, 1);
    NVIC_EnableIRQ  (TMR6_GLOBAL_IRQn);
    NVIC_SetPriority(EXINT15_4_IRQn, 0);
    NVIC_EnableIRQ  (EXINT15_4_IRQn);

    was_high = gpio_input_data_bit_read(rc_gpio, rc_pin);

    rc_find_arming_vals(RC_MEASURE_MULTIPLIER, GPIO_RC_PULSE_OFFSET, (uint16_t*)&arming_val_min, (uint16_t*)&arming_val_max);
}
