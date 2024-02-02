#include "cereal.h"

#ifdef ENABLE_COMPILE_CLI

#include "stm32_at32_compat.h"
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

#endif
