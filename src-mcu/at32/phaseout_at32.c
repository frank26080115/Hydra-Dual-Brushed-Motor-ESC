#include "phaseout.h"

#ifndef PWM_ENABLE_BRIDGE
#ifdef USE_INVERTED_LOW
    #define LOW_BITREG_ON  clr
    #define LOW_BITREG_OFF scr
#else
    #define LOW_BITREG_ON  scr
    #define LOW_BITREG_OFF clr
#endif

#ifdef USE_INVERTED_HIGH
    #define HIGH_BITREG_OFF scr
#else
    #define HIGH_BITREG_OFF clr
#endif
#endif

void pwm_init()
{
    PWMOUTTIMx->pr  = PWM_DEFAULT_PERIOD;
    PWMOUTTIMx->div = 0;
    PWMOUTTIMx->cm1 = 0x6868;   // Channel 1 and 2 in PWM output mode
    PWMOUTTIMx->cm2 = 0x68;     // channel 3 in PWM output mode

#ifdef USE_INVERTED_HIGH
    tmr_output_channel_polarity_set(PWMOUTTIMx, TMR_SELECT_CHANNEL_1, TMR_POLARITY_ACTIVE_LOW);
    tmr_output_channel_polarity_set(PWMOUTTIMx, TMR_SELECT_CHANNEL_2, TMR_POLARITY_ACTIVE_LOW);
    tmr_output_channel_polarity_set(PWMOUTTIMx, TMR_SELECT_CHANNEL_3, TMR_POLARITY_ACTIVE_LOW);
#endif

    tmr_output_channel_buffer_enable(PWMOUTTIMx, TMR_SELECT_CHANNEL_1, TRUE);
    tmr_output_channel_buffer_enable(PWMOUTTIMx, TMR_SELECT_CHANNEL_2, TRUE);
    tmr_output_channel_buffer_enable(PWMOUTTIMx, TMR_SELECT_CHANNEL_3, TRUE);

    tmr_period_buffer_enable(PWMOUTTIMx, TRUE);
    PWMOUTTIMx->brk_bit.dtc = DEAD_TIME;

    gpio_mode_QUICK(PHASE_A_GPIO_PORT_LOW, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_A_GPIO_LOW);
    gpio_mode_QUICK(PHASE_B_GPIO_PORT_LOW, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_B_GPIO_LOW);
    gpio_mode_QUICK(PHASE_C_GPIO_PORT_LOW, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_C_GPIO_LOW);

    gpio_pin_mux_config(PHASE_A_GPIO_PORT_LOW, PHASE_A_PIN_SOURCE_LOW, GPIO_MUX_2);
    gpio_pin_mux_config(PHASE_B_GPIO_PORT_LOW, PHASE_B_PIN_SOURCE_LOW, GPIO_MUX_2);
    gpio_pin_mux_config(PHASE_C_GPIO_PORT_LOW, PHASE_C_PIN_SOURCE_LOW, GPIO_MUX_2);

    gpio_mode_QUICK(PHASE_A_GPIO_PORT_HIGH, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_A_GPIO_HIGH);
    //gpio_output_options_set(PHASE_A_GPIO_PORT_HIGH, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PHASE_A_GPIO_HIGH);
    gpio_mode_QUICK(PHASE_B_GPIO_PORT_HIGH, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_B_GPIO_HIGH);
    //gpio_output_options_set(PHASE_B_GPIO_PORT_HIGH, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PHASE_B_GPIO_HIGH);
    gpio_mode_QUICK(PHASE_C_GPIO_PORT_HIGH, GPIO_MODE_MUX, GPIO_PULL_NONE, PHASE_C_GPIO_HIGH);
    //gpio_output_options_set(PHASE_C_GPIO_PORT_HIGH, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, PHASE_C_GPIO_HIGH);

    gpio_pin_mux_config(PHASE_A_GPIO_PORT_HIGH, PHASE_A_PIN_SOURCE_HIGH, GPIO_MUX_2);
    gpio_pin_mux_config(PHASE_B_GPIO_PORT_HIGH, PHASE_B_PIN_SOURCE_HIGH, GPIO_MUX_2);
    gpio_pin_mux_config(PHASE_C_GPIO_PORT_HIGH, PHASE_C_PIN_SOURCE_HIGH, GPIO_MUX_2);

    tmr_channel_enable(PWMOUTTIMx, TMR_SELECT_CHANNEL_1 , TRUE);
    tmr_channel_enable(PWMOUTTIMx, TMR_SELECT_CHANNEL_2 , TRUE);
    tmr_channel_enable(PWMOUTTIMx, TMR_SELECT_CHANNEL_3 , TRUE);
    tmr_channel_enable(PWMOUTTIMx, TMR_SELECT_CHANNEL_1C, TRUE);
    tmr_channel_enable(PWMOUTTIMx, TMR_SELECT_CHANNEL_2C, TRUE);
    tmr_channel_enable(PWMOUTTIMx, TMR_SELECT_CHANNEL_3C, TRUE);

    PWMOUTTIMx->ctrl1_bit.tmren = TRUE;
    PWMOUTTIMx->brk_bit.oen = TRUE;
    PWMOUTTIMx->swevt |= TMR_OVERFLOW_SWTRIG;
}

void pwm_set_period(uint32_t x)
{
    PWMOUTTIMx->pr = x;
}

#include "stm32_at32_compat.h"
#include "phaseout_shared.h"
