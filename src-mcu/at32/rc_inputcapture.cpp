#include "rc.h"

#include "rc_inputcapture_shared.h"

void rc_ic_tim_init(void)
{
    tmr_clock_source_div_set(RC_IC_TIMx, TMR_CLOCK_DIV1);
    RC_IC_TIMx->div = (SystemCoreClock / 1000000) - 1; // 1us resolution
    RC_IC_TIMx->pr = -1;

    // Set C1C=2‘b01 to set C1IN as C1IFP1;
    // Set C1P=1’b0 to set C1IFP1 rising edge active;
    // Set C2C=2‘b10 to set C2IN as C1IFP2;
    // Set C2P=1’b1 to set C1IFP2 falling edge active;
    tmr_input_config_type cfg;
    tmr_input_default_para_init(&cfg);
    cfg.input_channel_select  = TMR_SELECT_CHANNEL_1;
    cfg.input_polarity_select = TMR_INPUT_RISING_EDGE;
    cfg.input_mapped_select   = TMR_CC_CHANNEL_MAPPED_DIRECT;
    cfg.input_filter_value    = 1;
    tmr_pwm_input_config(RC_IC_TIMx, &cfg, TMR_CHANNEL_INPUT_DIV_1);
    // Set STIS=3’b101 to set C1IFP1 as the slave timer trigger signal;
    tmr_trigger_input_select(RC_IC_TIMx, TMR_SUB_INPUT_SEL_C1DF1);
    // Set SMSEL=3‘b100 to set the slave timer in reset mode;
    tmr_sub_mode_select(RC_IC_TIMx, TMR_SUB_RESET_MODE);
    // Set C1EN=1’b1 and C2EN=1’b1 to enable channel 1 and input capture.
}

void rc_ic_tim_init_2(void)
{
    gpio_init_type GPIO_InitStruct = { 0 };
    GPIO_InitStruct.gpio_mode           = GPIO_MODE_MUX;
    GPIO_InitStruct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    GPIO_InitStruct.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
    GPIO_InitStruct.gpio_pull           = GPIO_PULL_NONE;

    #if defined(MAIN_SIGNAL_PB4)
    GPIO_InitStruct.gpio_pins = INPUT_PIN;
    gpio_init(INPUT_PIN_PORT, &GPIO_InitStruct); // GPIOB
    gpio_pin_mux_config(INPUT_PIN_PORT, GPIO_PINS_SOURCE4, GPIO_MUX_1);
    NVIC_SetPriority(TMR3_GLOBAL_IRQn, 1);
    NVIC_EnableIRQ(TMR3_GLOBAL_IRQn);
    #elif defined(MAIN_SIGNAL_PA2)
    GPIO_InitStruct.gpio_pins = INPUT_PIN;
    gpio_init(INPUT_PIN_PORT, &GPIO_InitStruct); // GPIOA
    gpio_pin_mux_config(INPUT_PIN_PORT, GPIO_PINS_SOURCE2, GPIO_MUX_0);
    NVIC_SetPriority(TMR15_GLOBAL_IRQn, 1);
    NVIC_EnableIRQ(TMR15_GLOBAL_IRQn);
    #endif

    tmr_counter_enable(RC_IC_TIMx, TRUE);
}

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MAIN_SIGNAL_PB4) || INPUT_PIN == LL_GPIO_PIN_6
#define RcPulse_IQRHandler TMR3_GLOBAL_IRQHandler
#elif defined(MAIN_SIGNAL_PA2)
#define RcPulse_IQRHandler TMR15_GLOBAL_IRQHandler
#endif

void RcPulse_IQRHandler(void)
{
    #ifdef ENABLE_COMPILE_CLI
    if (ictimer_modeIsPulse)
    #endif
    {
        dbg_evntcnt_add(DBGEVNTID_ICTIMER);

        uint32_t p = RC_IC_TIMx->c1dt;   // Pulse period
        uint32_t w = RC_IC_TIMx->c2dt;   // Pulse width

        // reading the registers should automatically clear the interrupt flags

        if (p < RC_INPUT_VALID_MAX || w < RC_INPUT_VALID_MIN || w > RC_INPUT_VALID_MAX) // out of range
        {
            rc_register_bad_pulse((uint8_t*)&good_pulse_cnt, (uint8_t*)&bad_pulse_cnt, (uint32_t*)&arm_pulse_cnt);
        }
        else
        {
            pulse_width = w;

            RCPULSE_LOGJITTER();

            rc_register_good_pulse(
                pulse_width
                , arming_val_min, arming_val_max
                , (uint32_t*)&last_good_time
                , (uint8_t*)&good_pulse_cnt, (uint8_t*)&bad_pulse_cnt, (uint32_t*)&arm_pulse_cnt
                , (bool*)&new_flag, (bool*)&armed
            );
        }
    }
    #ifdef ENABLE_COMPILE_CLI
    else
    {
        CerealBitbang_IRQHandler();
    }
    #endif
}

#ifdef __cplusplus
}
#endif

RcPulse_InputCap::RcPulse_InputCap(tmr_type* TIMx, gpio_type* GPIOx, uint32_t pin, uint32_t chan)
{
    _tim  = TIMx;
    _gpio = GPIOx;
    _pin  = pin;
    _chan = chan;
}

void RcPulse_InputCap::init(void)
{
    rc_ic_tim_init();

    #ifdef ENABLE_COMPILE_CLI
    ictimer_modeIsPulse = true;
    #endif

    rc_ic_tim_init_2();

    rc_find_arming_vals(1, 0, (uint16_t*)&arming_val_min, (uint16_t*)&arming_val_max);
}
