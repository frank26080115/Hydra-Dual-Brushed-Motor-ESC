#include "cereal.h"

#ifdef ENABLE_COMPILE_CLI

#include "cereal_timer_shared.h"

#ifdef __cplusplus
extern "C" {
#endif

static void rx_mode(void)
{
    gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_MUX, GPIO_PULL_UP, INPUT_PIN);
    RC_IC_TIMx->stctrl = (0x4 << 0) | (0x4 << 4);           // Reset on any edge on TI1
    RC_IC_TIMx->cctrl  = (1 << 4) | (1 << 5);               // IC2 on falling edge on TI1
    tmr_flag_clear(RC_IC_TIMx, TMR_C2_FLAG);                // Clear flag
    tmr_interrupt_enable(RC_IC_TIMx, TMR_C2_INT, TRUE);     // Enable capture/compare 2 interrupt
    RC_IC_TIMx->c1dt = CLK_CNT(cereal_baud * 2);            // Half-bit time
    tmr_event_sw_trigger(RC_IC_TIMx, TMR_OVERFLOW_SWTRIG);  // Reinitialize the counter and generates an update of the registers
}

void CerealBitbang_IRQHandler(void)
{
    dbg_evntcnt_add(DBGEVNTID_BITBANG);

    #define AT32_TMR_FLAG_GET(_x_)    (tmr_flag_get(RC_IC_TIMx, (_x_)) && ((RC_IC_TIMx->iden & (_x_)) != 0))

    static volatile uint8_t n = 0, b;
    if (AT32_TMR_FLAG_GET(TMR_OVF_FLAG)) // each period is one bit width
    {
        // the TX function has already launched the start bit, this event happens after the start bit has been sent
        tmr_flag_clear(RC_IC_TIMx, TMR_OVF_FLAG);
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
            RC_IC_TIMx->c1dt = p; // Generate the pulse
            return;
        }

        rx_is_busy = false;
        rx_mode();
    }
    else if (AT32_TMR_FLAG_GET(TMR_C1_FLAG)) // each period is one bit width
    {
        // the first occurrence of this event happens in the middle of the first start bit
        tmr_flag_clear(RC_IC_TIMx, TMR_C1_FLAG);
        flag_status p = gpio_input_data_bit_read(INPUT_PIN_PORT, INPUT_PIN); // Signal level
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
        rx_is_busy = false;
        rx_mode();
    }
    else if (AT32_TMR_FLAG_GET(TMR_C2_FLAG))
    {
        // receiving
        // this event happens on the falling edge of the start bit
        tmr_flag_clear(RC_IC_TIMx, TMR_C1_FLAG);
        tmr_interrupt_enable(RC_IC_TIMx, TMR_C1_INT, TRUE);
        tmr_interrupt_enable(RC_IC_TIMx, TMR_C2_INT, FALSE);
        tmr_flag_clear(RC_IC_TIMx, TMR_C2_FLAG);
        tmr_interrupt_enable(RC_IC_TIMx, TMR_OVF_INT, TRUE);
        tmr_flag_clear(RC_IC_TIMx, TMR_OVF_FLAG);
        RC_IC_TIMx->stctrl = 0;
        RC_IC_TIMx->cctrl = 0;
        gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_INPUT, GPIO_PULL_UP, INPUT_PIN);
        rx_is_busy = true;
    }
}

#ifdef __cplusplus
}
#endif

void Cereal_TimerBitbang::init(uint32_t baud)
{
    sw_init(baud);
    RC_IC_TIMx->cctrl = 0;
    RC_IC_TIMx->cm1   = (1 << 3) | (0x07 << 4) | (0x2 << 8) | (0x03 << 12);
    RC_IC_TIMx->div   = 0;
    RC_IC_TIMx->pr    = CLK_CNT(baud) - 1;  // Bit time
    RC_IC_TIMx->c1dt  = CLK_CNT(baud * 2);  // Half-bit time

    ictimer_modeIsPulse = false;
    rc_ic_tim_init_2();

    __disable_irq();
    rx_mode();
    __enable_irq();
}

void Cereal_TimerBitbang::write(uint8_t x)
{
    fifo_push(fifo_tx, x);
    while (rx_is_busy) {
        // do nothing but wait
    }
    __disable_irq();
    if (tx_is_busy == false || ((RC_IC_TIMx->iden & (TMR_OVF_INT)) == 0))
    {
        // trigger the first bit, the interrupt will pop out the byte and start sending
        tx_is_busy = true;
        gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_MUX, GPIO_PULL_UP, INPUT_PIN);
        tmr_counter_enable(RC_IC_TIMx, FALSE);
        tmr_counter_value_set(RC_IC_TIMx, 0);
        RC_IC_TIMx->stctrl = 0;
        RC_IC_TIMx->c1dt = 0;             // Preload high level
        tmr_event_sw_trigger(RC_IC_TIMx, TMR_OVERFLOW_SWTRIG); // Update registers and trigger UEV
        RC_IC_TIMx->cctrl = (1 << 0);     // Enable output
        tmr_interrupt_enable(RC_IC_TIMx, TMR_C1_INT, TRUE);
        tmr_interrupt_enable(RC_IC_TIMx, TMR_C2_INT, TRUE);
        tmr_flag_clear(RC_IC_TIMx, TMR_OVF_FLAG);
        tmr_interrupt_enable(RC_IC_TIMx, TMR_OVF_INT, TRUE);
        tmr_counter_enable(RC_IC_TIMx, TRUE);
    }
    __enable_irq();
    if (x == '\n') {
        flush();
    }
}

#endif
