#include "ws2812.h"
#include "led.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef USE_LED_STRIP

#define WS2812_DMAx   DMA1
#define WS2812_TIMx   TIM16
#define WS2812_DMA_CH LL_DMA_CHANNEL_6
#define WS2812_TIM_CH LL_TIM_CHANNEL_CH1

#define WS2812_LED_BUFF_LEN 28

static volatile bool dma_busy = false;
static uint16_t led_buffer[WS2812_LED_BUFF_LEN] = {
     0,  0,
    20, 20, 20, 20, 20, 20, 20, 20,
    60, 60, 60, 60, 60, 60, 60, 60,
    20, 20, 20, 20, 20, 20, 20, 20,
     0,  0,
};

static uint8_t rgb_pending[3] = {0};
static bool    new_pending = false;

void WS2812_init(void)
{
    NVIC_SetPriority(DMA1_Ch4_7_DMAMUX1_OVR_IRQn, 3);
    NVIC_EnableIRQ  (DMA1_Ch4_7_DMAMUX1_OVR_IRQn);

    LL_TIM_InitTypeDef      TIM_InitStruct     = {0};
    LL_TIM_OC_InitTypeDef   TIM_OC_InitStruct  = {0};
    LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};
    LL_GPIO_InitTypeDef     GPIO_InitStruct    = {0};

    LL_DMA_SetPeriphRequest        (WS2812_DMAx, WS2812_DMA_CH, LL_DMAMUX_REQ_TIM16_CH1);
    LL_DMA_SetDataTransferDirection(WS2812_DMAx, WS2812_DMA_CH, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetChannelPriorityLevel (WS2812_DMAx, WS2812_DMA_CH, LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetMode                 (WS2812_DMAx, WS2812_DMA_CH, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode        (WS2812_DMAx, WS2812_DMA_CH, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode        (WS2812_DMAx, WS2812_DMA_CH, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize           (WS2812_DMAx, WS2812_DMA_CH, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize           (WS2812_DMAx, WS2812_DMA_CH, LL_DMA_MDATAALIGN_HALFWORD);

    TIM_InitStruct.Prescaler         = 0;
    TIM_InitStruct.CounterMode       = LL_TIM_COUNTERMODE_DOWN;
    TIM_InitStruct.Autoreload        = 79;
    TIM_InitStruct.ClockDivision     = LL_TIM_CLOCKDIVISION_DIV1;
    TIM_InitStruct.RepetitionCounter = 0;
    LL_TIM_Init            (WS2812_TIMx, &TIM_InitStruct);
    LL_TIM_EnableARRPreload(WS2812_TIMx);
    LL_TIM_OC_EnablePreload(WS2812_TIMx, WS2812_TIM_CH);
    TIM_OC_InitStruct.OCMode       = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState      = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState     = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity   = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCNPolarity  = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCIdleState  = LL_TIM_OCIDLESTATE_LOW;
    TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init       (WS2812_TIMx, WS2812_TIM_CH, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(WS2812_TIMx, WS2812_TIM_CH);
    TIM_BDTRInitStruct.OSSRState       = LL_TIM_OSSR_DISABLE;
    TIM_BDTRInitStruct.OSSIState       = LL_TIM_OSSI_DISABLE;
    TIM_BDTRInitStruct.LockLevel       = LL_TIM_LOCKLEVEL_OFF;
    TIM_BDTRInitStruct.DeadTime        = 0;
    TIM_BDTRInitStruct.BreakState      = LL_TIM_BREAK_DISABLE;
    TIM_BDTRInitStruct.BreakPolarity   = LL_TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStruct.BreakFilter     = LL_TIM_BREAK_FILTER_FDIV1;
    TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
    LL_TIM_BDTR_Init(WS2812_TIMx, &TIM_BDTRInitStruct);

    //  NVIC_SetPriority(TIM16_IRQn, 0);
    //  NVIC_EnableIRQ(TIM16_IRQn);

    GPIO_InitStruct.Pin        = LL_GPIO_PIN_8;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_2;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    WS2812_TIMx->CCER |=  1 << 0;
}

void WS2812_sendDMA(void)
{
    dma_busy = true;
    WS2812_TIMx->CNT = 0;
    LL_DMA_ConfigAddresses (WS2812_DMAx, LL_DMA_CHANNEL_6, (uint32_t)&led_buffer, (uint32_t)&TIM16->CCR1, LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_6));
    LL_DMA_SetDataLength   (WS2812_DMAx, LL_DMA_CHANNEL_6, WS2812_LED_BUFF_LEN);
    LL_DMA_EnableIT_TC     (WS2812_DMAx, LL_DMA_CHANNEL_6);
    LL_DMA_EnableIT_TE     (WS2812_DMAx, LL_DMA_CHANNEL_6);
    LL_DMA_EnableChannel   (WS2812_DMAx, LL_DMA_CHANNEL_6);
    LL_TIM_EnableDMAReq_CC1(WS2812_TIMx);
    LL_TIM_CC_EnableChannel(WS2812_TIMx, WS2812_TIM_CH);
    LL_TIM_EnableAllOutputs(WS2812_TIMx);
    LL_TIM_EnableCounter   (WS2812_TIMx);
}

void WS2812_setRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    rgb_pending[1] = red; rgb_pending[2] = green; rgb_pending[0] = blue;
    new_pending = true;
}

void WS2812_task(void)
{
    if (new_pending && !dma_busy)
    {
        uint32_t twenty_four_bit_color_number = (uint32_t)*((uint32_t*)rgb_pending);

        for (int i = 0; i < 24 ; i ++) {
            led_buffer[i + 2] = (((twenty_four_bit_color_number >> (23 - i)) & 1) * 40) + 20;
        }
        WS2812_sendDMA();
        new_pending = false;
    }
}

// NVIC is not called to enable this particular handler, so it is never called
#if 0
void TIM16_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_CC1(TIM16) == 1) {
        LL_TIM_ClearFlag_CC1(TIM16);
    }
    if (LL_TIM_IsActiveFlag_UPDATE(TIM16) == 1) {
        LL_TIM_ClearFlag_UPDATE(TIM16);
    }
}
#endif

// this interrupt is actually enabled
// this won't be too busy, once every 100 milliseconds, with lowest priority
// it should not cause jitter for the RC input
void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_HT6(WS2812_DMAx)) {
    }
    if (LL_DMA_IsActiveFlag_TC6(WS2812_DMAx) == 1) {
        LL_DMA_ClearFlag_GI6(WS2812_DMAx);

        LL_DMA_DisableChannel(WS2812_DMAx, WS2812_DMA_CH);
        // LL_TIM_CC_DisableChannel(WS2812_TIMx, WS2812_DMA_CH);
        LL_TIM_DisableAllOutputs(WS2812_TIMx);
        LL_TIM_DisableCounter(WS2812_TIMx);
        dma_busy = false;

    }
    else if (LL_DMA_IsActiveFlag_TE6(WS2812_DMAx) == 1) {
        LL_DMA_DisableChannel(WS2812_DMAx, WS2812_DMA_CH);
        // LL_TIM_CC_DisableChannel(TIM16, WS2812_DMA_CH);
        LL_TIM_DisableAllOutputs(WS2812_TIMx);
        LL_TIM_DisableCounter(WS2812_TIMx);
        dma_busy = false;
        LL_DMA_ClearFlag_GI6(WS2812_DMAx);
    }
}

#endif
