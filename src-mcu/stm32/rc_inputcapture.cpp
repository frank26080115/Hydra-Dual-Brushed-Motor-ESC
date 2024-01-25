#include "rc_stm32.h"

#define RC_IC_TIMx IC_TIMER_REGISTER

static volatile uint16_t pulse_width;
static volatile bool     new_flag       = false;
static volatile uint32_t last_good_time = 0;
static volatile uint8_t  good_pulse_cnt = 0;
static volatile uint8_t  bad_pulse_cnt  = 0;
static volatile uint32_t arm_pulse_cnt  = 0;
static volatile bool     armed          = false;
static uint16_t arming_val_min = 0, arming_val_max = 0;

void rc_ic_tim_init(void)
{
    RC_IC_TIMx->BDTR  = TIM_BDTR_MOE;
    RC_IC_TIMx->SMCR  = TIM_SMCR_SMS_RM | TIM_SMCR_TS_TI1FP1; // Reset on rising edge on TI1
    RC_IC_TIMx->CCMR1 =
    #if LL_TIM_CHANNEL_CH1 == IC_TIMER_CHANNEL
        TIM_CCMR1_CC1S_IN_TI1 | TIM_CCMR1_IC1F_CK_INT_N_8
    #elif LL_TIM_CHANNEL_CH2 == IC_TIMER_CHANNEL
        TIM_CCMR1_CC2S_IN_TI1 | TIM_CCMR1_IC2F_CK_INT_N_8
    #elif LL_TIM_CHANNEL_CH3 == IC_TIMER_CHANNEL
        TIM_CCMR1_CC3S_IN_TI1 | TIM_CCMR1_IC3F_CK_INT_N_8
    #elif LL_TIM_CHANNEL_CH4 == IC_TIMER_CHANNEL
        TIM_CCMR1_CC4S_IN_TI1 | TIM_CCMR1_IC4F_CK_INT_N_8
    #endif
        ;
    RC_IC_TIMx->CCER  = 
    #if LL_TIM_CHANNEL_CH1 == IC_TIMER_CHANNEL
        TIM_CCER_CC1E
    #elif LL_TIM_CHANNEL_CH2 == IC_TIMER_CHANNEL
        TIM_CCER_CC2E
    #elif LL_TIM_CHANNEL_CH3 == IC_TIMER_CHANNEL
        TIM_CCER_CC3E
    #elif LL_TIM_CHANNEL_CH4 == IC_TIMER_CHANNEL
        TIM_CCER_CC4E
    #endif
        ;
    RC_IC_TIMx->DIER  = TIM_DIER_UIE |
    #if LL_TIM_CHANNEL_CH1 == IC_TIMER_CHANNEL
        TIM_DIER_CC1IE
    #elif LL_TIM_CHANNEL_CH2 == IC_TIMER_CHANNEL
        TIM_DIER_CC2IE
    #elif LL_TIM_CHANNEL_CH3 == IC_TIMER_CHANNEL
        TIM_DIER_CC3IE
    #elif LL_TIM_CHANNEL_CH4 == IC_TIMER_CHANNEL
        TIM_DIER_CC4IE
    #endif
        ;
    RC_IC_TIMx->PSC = (SystemCoreClock / 1000000) - 1; // 1us resolution
    RC_IC_TIMx->ARR = -1;
    RC_IC_TIMx->CR1 = TIM_CR1_URS;
    RC_IC_TIMx->EGR = TIM_EGR_UG;
    RC_IC_TIMx->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;
}

void rc_ic_tim_init_2(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

    #if INPUT_PIN == LL_GPIO_PIN_4 || INPUT_PIN == LL_GPIO_PIN_6
    GPIO_InitStruct.Pin = INPUT_PIN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
    LL_GPIO_Init(INPUT_PIN_PORT, &GPIO_InitStruct); // GPIOB
    NVIC_SetPriority(TIM3_IRQn, 0);
    NVIC_EnableIRQ(TIM3_IRQn);
    #elif INPUT_PIN == LL_GPIO_PIN_2
    GPIO_InitStruct.Pin = INPUT_PIN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(INPUT_PIN_PORT, &GPIO_InitStruct); // GPIOA
    NVIC_SetPriority(TIM15_IRQn, 0);
    NVIC_EnableIRQ(TIM15_IRQn);
    #endif

    RC_IC_TIMx->CR1 |= TIM_CR1_CEN; // enable timer
}

#ifdef ENABLE_COMPILE_CLI
bool ictimer_modeIsPulse;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if INPUT_PIN == LL_GPIO_PIN_4 || INPUT_PIN == LL_GPIO_PIN_6
#define RcPulse_IQRHandler TIM3_IRQHandler
#elif INPUT_PIN == LL_GPIO_PIN_2
#define RcPulse_IQRHandler TIM15_IRQHandler
#endif

#ifdef ENABLE_COMPILE_CLI
extern void CerealBitbang_IRQHandler(void);
#endif

void RcPulse_IQRHandler(void)
{
    #ifdef ENABLE_COMPILE_CLI
    if (ictimer_modeIsPulse)
    #endif
    {
        dbg_evntcnt_add(DBGEVNTID_ICTIMER);

        uint32_t p = RC_IC_TIMx->CCR1;   // Pulse period
        uint32_t w = RC_IC_TIMx->CCR2;   // Pulse width

        // reading the registers should automatically clear the interrupt flags

        if (p < RC_INPUT_VALID_MAX || w < RC_INPUT_VALID_MIN || w > RC_INPUT_VALID_MAX)
        {
            arm_pulse_cnt = 0;
            if (bad_pulse_cnt < 3) {
                bad_pulse_cnt++;
            }
            else {
                good_pulse_cnt = 0;
            }
        }
        else
        {
            pulse_width = w;
            last_good_time = millis();
            good_pulse_cnt++;
            bad_pulse_cnt = 0;
            new_flag = true;
            if (arm_pulses_required > 0)
            {
                if (w >= arming_val_min && w <= arming_val_max) {
                    arm_pulse_cnt++;
                    if (arm_pulse_cnt >= arm_pulses_required) {
                        armed = true;
                    }
                }
                else {
                    arm_pulse_cnt = 0;
                }
            }
            else {
                armed = true;
            }
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

RcPulse_InputCap::RcPulse_InputCap(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin, uint32_t chan)
{
    _tim  = TIMx;
    _gpio = GPIOx;
    _pin  = pin;
    _chan = chan;
}

void RcPulse_InputCap::init(void)
{
    rc_ic_tim_init();

    // STM32F051 tech ref. manual Appendix A.9.5 has example code

    RC_IC_TIMx->CCMR1 = 
    #if LL_TIM_CHANNEL_CH1 == IC_TIMER_CHANNEL || LL_TIM_CHANNEL_CH2 == IC_TIMER_CHANNEL
        TIM_CCMR1_CC1S_IN_TI1 | TIM_CCMR1_IC1F_DTF_DIV_8_N_8 | TIM_CCMR1_CC2S_IN_TI1 | TIM_CCMR1_IC2F_DTF_DIV_8_N_8;
    #else
        #error not implemented
    #endif
    RC_IC_TIMx->CCER  = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC2P; // IC1 on rising edge on TI1, IC2 on falling edge on TI1
    RC_IC_TIMx->SR    = ~TIM_SR_CC2IF;  // clear pending interrupt
    RC_IC_TIMx->DIER  = TIM_DIER_CC2IE; // enable interrupt

    #ifdef ENABLE_COMPILE_CLI
    ictimer_modeIsPulse = true;
    #endif

    rc_ic_tim_init_2();

    #if 0
    uint16_t test_arming_val = 0;
    int last_v = -THROTTLE_UNIT_RANGE;
    for (test_arming_val = 1250; test_arming_val < 1750; test_arming_val++) {
        int v = rc_pulse_map(test_arming_val);
        if (v == 0 && last_v < 0 && arming_val_min == 0) {
            arming_val_min = test_arming_val - 1;
        }
        if (v == 0 && last_v > 0 && arming_val_max == 0) {
            arming_val_max = test_arming_val;
            break;
        }
        last_v = v;
    }
    #else
    arming_val_min = 1450;
    arming_val_max = 1550;
    #endif
}

void RcPulse_InputCap::task(void)
{
    RcChannel::task();
}

int16_t RcPulse_InputCap::read(void)
{
    return rc_pulse_map(pulse_width);
}

int16_t RcPulse_InputCap::readRaw(void)
{
    return pulse_width;
}

bool RcPulse_InputCap::is_alive(void)
{
    if ((millis() - last_good_time) < RC_INPUT_TIMEOUT)
    {
        if (good_pulse_cnt >= 3) {
            return true;
        }
    }
    new_flag = false;
    arm_pulse_cnt = 0;
    return false;
}

bool RcPulse_InputCap::has_new(bool clr)
{
    bool x = new_flag;
    if (clr) {
        new_flag = false;
    }
    return x;
}

bool RcPulse_InputCap::is_armed(void)
{
    if ((millis() - last_good_time) < disarm_timeout || disarm_timeout <= 0)
    {
        return armed;
    }
    armed = false;
    arm_pulse_cnt = 0;
    return false;
}

void RcPulse_InputCap::disarm(void)
{
    armed = false;
    arm_pulse_cnt = 0;
}
