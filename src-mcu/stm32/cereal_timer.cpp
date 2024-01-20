#include "cereal_timer.h"

#define RC_IC_TIMx IC_TIMER_REGISTER

// only one instance of this cereal port is allowed
// so we use global static variables that can be access through ISRs

static uint32_t      cereal_baud;
static fifo_t*       cereal_fifo_tx;
static fifo_t*       cereal_fifo_rx;
static volatile uint32_t last_rx_time;

extern void rc_ic_tim_init(void);
extern void rc_ic_tim_init_2(void);
extern bool ictimer_modeIsPulse;

void CerealBitbang_IRQHandler(void)
{
    static int n = 0, b;
    if (LL_TIM_IsActiveFlag_UPDATE(RC_IC_TIMx))
    {
        LL_TIM_ClearFlag_UPDATE(RC_IC_TIMx);
        if (fifo_available(cereal_fifo_tx) > 0) {
            int p = -1;
            if (n == 0) { // Start bit
                b = fifo_pop(cereal_fifo_tx);
                n++;
            }
            else if (n < 10) { // Data bits
                if (b & 1) p = 0;
                b >>= 1;
            } else { // Stop bit
                n = 0;
                p = 0;
            }
            RC_IC_TIMx->CCR1 = p; // Generate the pulse
            return;
        }

        // Go into receive mode if no more data needs to be sent
        RC_IC_TIMx->SMCR = TIM_SMCR_SMS_RM | LL_TIM_TS_TI1F_ED; // Reset on any edge on TI1
        RC_IC_TIMx->CCER = TIM_CCER_CC2E | TIM_CCER_CC2P;       // IC2 on falling edge on TI1
        RC_IC_TIMx->SR   = ~TIM_SR_CC2IF;                       // Clear flag
        RC_IC_TIMx->DIER = TIM_DIER_CC2IE;                      // Enable capture/compare 2 interrupt
        RC_IC_TIMx->CCR1 = CLK_CNT(cereal_baud * 2);            // Half-bit time
        RC_IC_TIMx->EGR  = TIM_EGR_UG;
    }
    else if (LL_TIM_IsActiveFlag_CC1(RC_IC_TIMx))
    {
        LL_TIM_ClearFlag_CC1(RC_IC_TIMx);
        int p = LL_GPIO_IsInputPinSet(INPUT_PIN_PORT, INPUT_PIN); // Signal level
        if (n == 0) { // Start bit
            n++;
            //if (p) WWDG_CR = WWDG_CR_WDGA; // Data error
            b = 0;
            return;
        }
        if (n < 10) { // Data bit
            b >>= 1;
            if (p) {
                b |= 0x80;
            }
            return;
        }
        //if (!p || i == sizeof iobuf - 1) WWDG_CR = WWDG_CR_WDGA; // Data error
        RC_IC_TIMx->SR = ~TIM_SR_CC2IF;
        RC_IC_TIMx->DIER = TIM_DIER_CC2IE;
        n = 0;

        fifo_push(cereal_fifo_rx, b);

        RC_IC_TIMx->SMCR = 0;
        RC_IC_TIMx->CCR1 = 0; // Preload high level
        RC_IC_TIMx->EGR  = TIM_EGR_UG;    // Update registers and trigger UEV
        RC_IC_TIMx->CCER = TIM_CCER_CC1E; // Enable output
        RC_IC_TIMx->DIER = TIM_DIER_UIE;
    }
    else if (LL_TIM_IsActiveFlag_CC2(RC_IC_TIMx))
    {
        LL_TIM_ClearFlag_CC2(RC_IC_TIMx);
        LL_TIM_EnableIT_CC1(RC_IC_TIMx);
    }
}

Cereal_TimerBitbang::Cereal_TimerBitbang(uint8_t id, uint16_t sz)
{
    _id = id;
    _buff_size = sz;
}

void Cereal_TimerBitbang::begin(uint32_t baud)
{
    cereal_fifo_tx = fifo_init(_buff_size);
    cereal_fifo_rx = fifo_init(_buff_size);
    fifo_tx = cereal_fifo_tx;
    fifo_rx = cereal_fifo_rx;
    cereal_baud = baud;
    rc_ic_tim_init();
    RC_IC_TIMx->CCER  = 0;
    RC_IC_TIMx->SMCR  = TIM_SMCR_SMS_RM | TIM_SMCR_TS_TI1F_ED; // Reset on any edge on TI1
    RC_IC_TIMx->CCMR1 = TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_PWM2 | TIM_CCMR1_CC2S_IN_TI1 | TIM_CCMR1_IC2F_CK_INT_N_8;
    RC_IC_TIMx->CCER  = TIM_CCER_CC2E | TIM_CCER_CC2P; // IC2 on falling edge on TI1
    RC_IC_TIMx->SR    = ~TIM_SR_CC2IF;
    RC_IC_TIMx->DIER  = TIM_DIER_CC2IE;
    RC_IC_TIMx->PSC   = 0;
    RC_IC_TIMx->ARR   = CLK_CNT(baud) - 1;  // Bit time
    RC_IC_TIMx->CCR1  = CLK_CNT(baud * 2); // Half-bit time
    RC_IC_TIMx->EGR   = TIM_EGR_UG;
    RC_IC_TIMx->CR1   = TIM_CR1_CEN;

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin        = INPUT_PIN;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate  = 
    #if INPUT_PIN == LL_GPIO_PIN_2
        LL_GPIO_AF_0
    #elif INPUT_PIN == LL_GPIO_PIN_4
        LL_GPIO_AF_1
    #endif
        ;
    LL_GPIO_Init(INPUT_PIN_PORT, &GPIO_InitStruct);

    ictimer_modeIsPulse = false;
    rc_ic_tim_init_2();
}

void Cereal_TimerBitbang::write(uint8_t x)
{
    fifo_push(fifo_tx, x);
    if (x == '\n') {
        flush();
    }
}

void Cereal_TimerBitbang::flush(void)
{
    while (fifo_available(fifo_tx)) {
        // do nothing but wait
    }
}

uint32_t Cereal_TimerBitbang::get_last_time(void)
{
    return last_rx_time;
}

bool Cereal_TimerBitbang::get_idle_flag(bool clr)
{
    bool x = false;
    __disable_irq();
    x = (millis() - last_rx_time) > 100;
    __enable_irq();
    return x;
}
