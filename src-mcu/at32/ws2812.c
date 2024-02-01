#include "ws2812.h"
#include "led.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef USE_LED_STRIP

#define WS2812_TIMx   TIM16
#ifndef WS2812_PIN
#define WS2812_PIN    GPIO_PINS_7
#endif
#ifndef WS2812_PORT
#define WS2812_PORT   GPIOB
#endif

static uint8_t rgb_pending[3] = {0};
static bool    new_pending = false;

void WS2812_waitClkCycles(uint16_t cycles)
{
    WS2812_TIMx->cval = 0;
    while (WS2812_TIMx->cval < cycles) {
    }
}

void WS2812_sendBit(uint8_t bit)
{
    WS2812_PORT->scr = WS2812_PIN;
    waitClockCycles(CPU_FREQUENCY_MHZ >> (2 - bit));
    WS2812_PORT->clr = WS2812_PIN;
    waitClockCycles(CPU_FREQUENCY_MHZ >> (1 + bit));
}

void WS2812_init(void)
{
    WS2812_TIMx->ctrl1_bit.tmren = TRUE;
    gpio_mode_QUICK(WS2812_PORT, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, WS2812_PIN)
}

void WS2812_setRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_pending[1] = red; rgb_pending[2] = green; rgb_pending[0] = blue;
    new_pending = true;
}

void WS2812_onIrq(void)
{
    if (new_pending == false) {
        return;
    }
    new_pending = false;
    WS2812_TIMx->div = 0;
    WS2812_TIMx->swevt |= TMR_OVERFLOW_SWTRIG;
    uint32_t twenty_four_bit_color_number = *((uint32_t*)rgb_pending);
    for (int i = 0; i < 24 ; i ++) {
        WS2812_sendBit((twenty_four_bit_color_number >> (23 - i)) & 1);
    }
    WS2812_PORT->clr = WS2812_PIN;
    WS2812_TIMx->div = CPU_FREQUENCY_MHZ;
    WS2812_TIMx->swevt |= TMR_OVERFLOW_SWTRIG;
}

void WS2812_task(void)
{
    // do nothing, this is called from main app periodically
    // we want to be sending the bits only when a timer interrupt fires, to avoid jitter in pulse measurements
}

#endif
