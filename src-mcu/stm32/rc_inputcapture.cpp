#include "rc_stm32.h"

#define RC_IC_TIMx IC_TIMER_REGISTER

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
    RC_IC_TIMx->PSC   = (SystemCoreClock / 1000000) - 1; // 1us resolution
    RC_IC_TIMx->ARR   = -1;
    RC_IC_TIMx->CR1   = TIM_CR1_URS;
    RC_IC_TIMx->EGR   = TIM_EGR_UG;
    RC_IC_TIMx->CR1   = TIM_CR1_CEN | TIM_CR1_ARPE | TIM_CR1_URS;
}

void rc_ic_tim_init_2(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

    #if INPUT_PIN == LL_GPIO_PIN_4
    GPIO_InitStruct.Pin = INPUT_PIN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    NVIC_SetPriority(TIM3_IRQn, 0);
    NVIC_EnableIRQ(TIM3_IRQn);
    #elif INPUT_PIN == LL_GPIO_PIN_2
    GPIO_InitStruct.Pin = INPUT_PIN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    NVIC_SetPriority(TIM15_IRQn, 0);
    NVIC_EnableIRQ(TIM15_IRQn);
    #endif
}

bool ictimer_modeIsPulse;
extern void CerealBitbang_IRQHandler(void);

void RcPulse_IQRHandler(void)
{
    if (ictimer_modeIsPulse)
    {
        uint32_t p = RC_IC_TIMx->CCR1;   // Pulse period
        uint32_t w = RC_IC_TIMx->CCR2;   // Pulse width
        if (p < 2000) return;            // Invalid signal
        if (w < 800 || w > 2200) return; // Invalid signal
        w = rc_pulse_map(w);
    }
    else
    {
        CerealBitbang_IRQHandler();
    }
}

void RcPulse_InputCap::init(void)
{
    rc_ic_tim_init();
    RC_IC_TIMx->CCMR1 = 
    #if LL_TIM_CHANNEL_CH1 == IC_TIMER_CHANNEL || LL_TIM_CHANNEL_CH2 == IC_TIMER_CHANNEL
        TIM_CCMR1_CC1S_IN_TI1 | TIM_CCMR1_IC1F_DTF_DIV_8_N_8 | TIM_CCMR1_CC2S_IN_TI1 | TIM_CCMR1_IC2F_DTF_DIV_8_N_8;
    #else
        #error not implemented
    #endif
    RC_IC_TIMx->CCER  = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC2P; // IC1 on rising edge on TI1, IC2 on falling edge on TI1
    RC_IC_TIMx->SR    = ~TIM_SR_CC2IF;
    RC_IC_TIMx->DIER  = TIM_DIER_CC2IE;

    ictimer_modeIsPulse = true;
    rc_ic_tim_init_2();
}
