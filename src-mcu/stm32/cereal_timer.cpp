#include "cereal.h"

#ifdef ENABLE_COMPILE_CLI

#include "stm32_at32_compat.h"
#include "cereal_timer_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

static void rx_mode(void)
{
    LL_GPIO_SetPinMode(INPUT_PIN_PORT, INPUT_PIN, LL_GPIO_MODE_ALTERNATE);
    RC_IC_TIMx->SMCR = TIM_SMCR_SMS_RM | LL_TIM_TS_TI1F_ED; // Reset on any edge on TI1
    RC_IC_TIMx->CCER = TIM_CCER_CC2E | TIM_CCER_CC2P;       // IC2 on falling edge on TI1
    RC_IC_TIMx->SR   = ~TIM_SR_CC2IF;                       // Clear flag
    RC_IC_TIMx->DIER = TIM_DIER_CC2IE;                      // Enable capture/compare 2 interrupt
    RC_IC_TIMx->CCR1 = CLK_CNT(cereal_baud * 2);            // Half-bit time
    RC_IC_TIMx->EGR  = TIM_EGR_UG;                          // Reinitialize the counter and generates an update of the registers
}

#ifdef __cplusplus
}
#endif

void Cereal_TimerBitbang::init(uint32_t baud)
{
    sw_init(baud);
    RC_IC_TIMx->CCER  = 0;
    RC_IC_TIMx->SMCR  = TIM_SMCR_SMS_RM | TIM_SMCR_TS_TI1F_ED; // Reset on any edge on TI1
    RC_IC_TIMx->CCMR1 = TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_PWM2 | TIM_CCMR1_CC2S_IN_TI1 | TIM_CCMR1_IC2F_CK_INT_N_8;
    RC_IC_TIMx->CCER  = TIM_CCER_CC2E | TIM_CCER_CC2P; // IC2 on falling edge on TI1
    RC_IC_TIMx->SR    = ~TIM_SR_CC2IF;
    RC_IC_TIMx->DIER  = TIM_DIER_CC2IE;
    RC_IC_TIMx->PSC   = 0;
    RC_IC_TIMx->ARR   = CLK_CNT(baud) - 1;  // Bit time
    RC_IC_TIMx->CCR1  = CLK_CNT(baud * 2);  // Half-bit time
    RC_IC_TIMx->EGR   = TIM_EGR_UG;

    ictimer_modeIsPulse = false;
    rc_ic_tim_init_2();

    __disable_irq();
    rx_mode();
    __enable_irq();
}

#endif
