#include "cereal_timer.h"

#ifdef ENABLE_COMPILE_CLI

#define RC_IC_TIMx IC_TIMER_REGISTER

// only one instance of this cereal port is allowed
// so we use global static variables that can be access through ISRs

static uint32_t cereal_baud;
#ifdef ENABLE_CEREAL_TX
static fifo_t cereal_fifo_tx;
#endif
static fifo_t cereal_fifo_rx;
static volatile uint32_t last_rx_time = 0;
static volatile bool tx_is_busy = false;
static volatile bool idle_flag_cleared = false;

extern void rc_ic_tim_init(void);
extern void rc_ic_tim_init_2(void);
extern bool ictimer_modeIsPulse;

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

void CerealBitbang_IRQHandler(void)
{
    dbg_evntcnt_add(DBGEVNTID_BITBANG);

    static volatile uint8_t n = 0, b;
    if (LL_TIM_IsActiveFlag_UPDATE(RC_IC_TIMx) && LL_TIM_IsEnabledIT_UPDATE(RC_IC_TIMx)) // each period is one bit width
    {
        // the TX function has already launched the start bit, this event happens after the start bit has been sent
        LL_TIM_ClearFlag_UPDATE(RC_IC_TIMx);
        if (tx_is_busy) {
            int p = -1;
            if (n == 0) { // Start bit
                if (fifo_available(&cereal_fifo_tx) > 0) {
                    b = fifo_pop(&cereal_fifo_tx);
                    tx_is_busy = true;
                    n++;
                }
                else {
                    tx_is_busy = false;
                }
            }
            else if (n < 9) { // Data bits
                if (b & 1) {
                    p = 0;
                }
                b >>= 1;
                n++;
            } else { // Stop bit
                n = 0;
                p = 0;
            }
            RC_IC_TIMx->CCR1 = p; // Generate the pulse
            return;
        }

        rx_mode();
    }
    else if (LL_TIM_IsActiveFlag_CC1(RC_IC_TIMx) && LL_TIM_IsEnabledIT_CC1(RC_IC_TIMx)) // each period is one bit width
    {
        // the first occurrence of this event happens in the middle of the first start bit
        LL_TIM_ClearFlag_CC1(RC_IC_TIMx);
        int p = LL_GPIO_IsInputPinSet(INPUT_PIN_PORT, INPUT_PIN); // Signal level
        if (n == 0) { // Start bit
            n++;
            b = 0;
            return;
        }
        if (n < 9) { // Data bit
            b >>= 1;
            if (p) {
                b |= 0x80;
            }
            n++;
            if (n >= 9) {
                fifo_push(&cereal_fifo_rx, b);
                idle_flag_cleared = false;
            }
            return;
        }
        n = 0;
        rx_mode();
    }
    else if (LL_TIM_IsActiveFlag_CC2(RC_IC_TIMx) && LL_TIM_IsEnabledIT_CC2(RC_IC_TIMx))
    {
        // receiving
        // this event happens on the falling edge of the start bit
        LL_TIM_ClearFlag_CC1(RC_IC_TIMx);
        LL_TIM_ClearFlag_CC2(RC_IC_TIMx);
        LL_TIM_EnableIT_CC1(RC_IC_TIMx);
        LL_TIM_DisableIT_CC2(RC_IC_TIMx);
        LL_TIM_DisableIT_UPDATE(RC_IC_TIMx);
        RC_IC_TIMx->SMCR = 0; // ignore edges
        RC_IC_TIMx->CCER = 0; // no need for capture
        LL_GPIO_SetPinMode(INPUT_PIN_PORT, INPUT_PIN, LL_GPIO_MODE_INPUT);
    }
}

#ifdef __cplusplus
}
#endif

Cereal_TimerBitbang::Cereal_TimerBitbang(uint8_t id)
{
    _id = id;
}

void Cereal_TimerBitbang::init(uint32_t baud)
{
    fifo_init(&cereal_fifo_rx, cer_buff_1, CEREAL_BUFFER_SIZE);
    fifo_init(&cereal_fifo_tx, cer_buff_2, CEREAL_BUFFER_SIZE);
    fifo_tx = &cereal_fifo_tx;
    fifo_rx = &cereal_fifo_rx;
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
    RC_IC_TIMx->CCR1  = CLK_CNT(baud * 2);  // Half-bit time
    RC_IC_TIMx->EGR   = TIM_EGR_UG;

    ictimer_modeIsPulse = false;
    rc_ic_tim_init_2();

    __disable_irq();
    rx_mode();
    __enable_irq();
}

#ifdef ENABLE_CEREAL_TX
void Cereal_TimerBitbang::write(uint8_t x)
{
    fifo_push(fifo_tx, x);
    __disable_irq();
    if (tx_is_busy == false)
    {
        // trigger the first bit, the interrupt will pop out the byte and start sending
        tx_is_busy = true;
        LL_GPIO_SetPinMode(INPUT_PIN_PORT, INPUT_PIN, LL_GPIO_MODE_ALTERNATE);
        LL_TIM_DisableCounter(RC_IC_TIMx);
        LL_TIM_SetCounter(RC_IC_TIMx, 0);
        RC_IC_TIMx->SMCR = 0;
        RC_IC_TIMx->CCR1 = 0;             // Preload high level
        RC_IC_TIMx->EGR  = TIM_EGR_UG;    // Update registers and trigger UEV
        RC_IC_TIMx->CCER = TIM_CCER_CC1E; // Enable output
        LL_TIM_EnableIT_UPDATE(RC_IC_TIMx);
        LL_TIM_ClearFlag_UPDATE(RC_IC_TIMx);
        LL_TIM_EnableCounter(RC_IC_TIMx);
    }
    __enable_irq();
    if (x == '\n') {
        flush();
    }
}

void Cereal_TimerBitbang::flush(void)
{
    while (
        //fifo_available(fifo_tx) &&
        tx_is_busy) {
        // do nothing but wait
    }
}
#endif

uint32_t Cereal_TimerBitbang::get_last_time(void)
{
    return last_rx_time;
}

#ifdef ENABLE_CEREAL_IDLE_DETECT
bool Cereal_TimerBitbang::get_idle_flag(bool clr)
{
    // this implementation is never used, only data packet parsers use it
    // and those parsers always use USART, not bit-bang
    bool x = false;
    __disable_irq();
    x = (millis() - last_rx_time) > 100;
    if (x && idle_flag_cleared) {
        x = false;
    }
    if (clr) {
        idle_flag_cleared = true;
    }
    __enable_irq();
    return x;
}
#endif

#endif
