#include "cereal_timer.h"

// only one instance of this cereal port is allowed
// so we use global static variables that can be access through ISRs

static TIM_TypeDef*  cereal_tim;
static GPIO_TypeDef* cereal_gpio;
static uint32_t      cereal_pin;
static uint32_t      cereal_baud;
static fifo_t*       cereal_fifo_tx;
static fifo_t*       cereal_fifo_rx;

void CerealBitbang_IRQHandler(void)
{
    static int i, j, n, b;
    switch (cereal_tim->DIER) // Which interrupt enabled is kind of like our state machine
    {
        case TIM_DIER_UIE:    // We are writing
            cereal_tim->SR = ~TIM_SR_UIF;
            if (i < j) {
                int p = -1;
                if (n == 0) { // Start bit
                    b = fifo_pop(cereal_fifo_tx);
                    n++;
                }
                else if (n < 10) { // Data bits
                    if (b & 1) p = 0;
                    b >>= 1;
                } else { // Stop bit
                    if (++i == j) { // End of data
                        i = 0;
                        j = 0;
                    }
                    n = 0;
                    p = 0;
                }
                cereal_tim->CCR1 = p; // Generate the pulse
                break;
            }

            // Go into receive mode
            cereal_tim->SMCR = TIM_SMCR_SMS_RM | LL_TIM_TS_TI1F_ED; // Reset on any edge on TI1
            cereal_tim->CCER = TIM_CCER_CC2E | TIM_CCER_CC2P;       // IC2 on falling edge on TI1
            cereal_tim->SR   = ~TIM_SR_CC2IF;                       // Clear flag
            cereal_tim->DIER = TIM_DIER_CC2IE;                      // Enable capture/compare 2 interrupt
            cereal_tim->CCR1 = CLK_CNT(cereal_baud * 2);            // Half-bit time
            cereal_tim->EGR  = TIM_EGR_UG;
            break;

        case TIM_DIER_CC1IE: // Half-bit time
            cereal_tim->SR = ~TIM_SR_CC1IF; // Clear the flag
            int p = LL_GPIO_IsInputPinSet(cereal_gpio, cereal_pin); // Signal level
            if (n == 0) { // Start bit
                n++;
                //if (p) WWDG_CR = WWDG_CR_WDGA; // Data error
                b = 0;
                break;
            }
            if (n < 10) { // Data bit
                b >>= 1;
                if (p) {
                    b |= 0x80;
                }
                break;
            }
            //if (!p || i == sizeof iobuf - 1) WWDG_CR = WWDG_CR_WDGA; // Data error
            cereal_tim->SR = ~TIM_SR_CC2IF;
            cereal_tim->DIER = TIM_DIER_CC2IE;
            n = 0;

            fifo_push(cereal_fifo_rx, b);

            cereal_tim->SMCR = 0;
            cereal_tim->CCR1 = 0; // Preload high level
            cereal_tim->EGR  = TIM_EGR_UG;    // Update registers and trigger UEV
            cereal_tim->CCER = TIM_CCER_CC1E; // Enable output
            cereal_tim->DIER = TIM_DIER_UIE;
            break;

        case TIM_DIER_CC2IE: // Falling edge
            cereal_tim->SR = ~(TIM_SR_CC1IF | TIM_SR_CC2IF);
            cereal_tim->DIER = TIM_DIER_CC1IE;
            break;
    }
}

Cereal_TimerBitbang::Cereal_TimerBitbang(uint8_t id, TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin)
{
    _id = id;
    _tim = TIMx;
    cereal_tim = TIMx;
    _gpio = GPIOx;
    cereal_gpio = GPIOx;
    _pin = pin;
    cereal_pin = pin;
}

void Cereal_TimerBitbang::begin(uint32_t baud)
{
    rc_ic_tim_init();
    cereal_tim->CCER  = 0;
    cereal_tim->SMCR  = TIM_SMCR_SMS_RM | TIM_SMCR_TS_TI1F_ED; // Reset on any edge on TI1
    cereal_tim->CCMR1 = TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_PWM2 | TIM_CCMR1_CC2S_IN_TI1 | TIM_CCMR1_IC2F_CK_INT_N_8;
    cereal_tim->CCER  = TIM_CCER_CC2E | TIM_CCER_CC2P; // IC2 on falling edge on TI1
    cereal_tim->SR    = ~TIM_SR_CC2IF;
    cereal_tim->DIER  = TIM_DIER_CC2IE;
    cereal_tim->PSC   = 0;
    cereal_tim->ARR   = CLK_CNT(baud) - 1;  // Bit time
    cereal_tim->CCR1  = CLK_CNT(baud * 2); // Half-bit time
    cereal_tim->EGR   = TIM_EGR_UG;
    cereal_tim->CR1   = TIM_CR1_CEN;
}