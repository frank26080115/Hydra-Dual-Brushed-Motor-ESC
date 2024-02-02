#pragma once

#include "mcu.h"

#if defined(ARTERY)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SET_BIT
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#endif
#ifndef CLEAR_BIT
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#endif
#ifndef READ_BIT
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#endif
#ifndef CLEAR_REG
#define CLEAR_REG(REG)        ((REG) = (0x0))
#endif
#ifndef WRITE_REG
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#endif
#ifndef READ_REG
#define READ_REG(REG)         ((REG))
#endif
#ifndef MODIFY_REG
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#endif

#ifndef USART_TypeDef
#define USART_TypeDef usart_type
#endif
#ifndef GPIO_TypeDef
#define GPIO_TypeDef  gpio_type
#endif
#ifndef TIM_TypeDef
#define TIM_TypeDef   tmr_type
#endif

#ifndef LL_GPIO_IsInputPinSet
#define LL_GPIO_IsInputPinSet    gpio_input_data_bit_read
#endif

#ifndef GPIO_MODER_MODE0
#define GPIO_MODER_MODE0 0x03
#endif
#ifndef GPIO_PUPDR_PUPDR0
#define GPIO_PUPDR_PUPDR0 0x03
#endif

#ifndef LL_GPIO_SetPinMode
static inline void LL_GPIO_SetPinMode(gpio_type *GPIOx, uint32_t Pin, uint32_t Mode) {
  MODIFY_REG(GPIOx->cfgr, ((Pin * Pin) * GPIO_MODER_MODE0), ((Pin * Pin) * Mode));
}
#endif
#ifndef LL_GPIO_SetPinPull
static inline void LL_GPIO_SetPinPull(gpio_type *GPIOx, uint32_t Pin, uint32_t Pull) {
  MODIFY_REG(GPIOx->pull, ((Pin * Pin) * GPIO_PUPDR_PUPDR0), ((Pin * Pin) * Pull));
}
#endif
#ifndef LL_GPIO_ResetOutputPin
#define LL_GPIO_ResetOutputPin    gpio_bits_reset
#endif
#ifndef LL_GPIO_SetOutputPin
#define LL_GPIO_SetOutputPin      gpio_bits_set
#endif

#ifndef LL_GPIO_MODE_ALTERNATE
#define LL_GPIO_MODE_ALTERNATE    GPIO_MODE_MUX
#endif
#ifndef LL_GPIO_MODE_INPUT
#define LL_GPIO_MODE_INPUT        GPIO_MODE_INPUT
#endif
#ifndef LL_GPIO_MODE_OUTPUT
#define LL_GPIO_MODE_OUTPUT       GPIO_MODE_OUTPUT
#endif
#ifndef LL_GPIO_MODE_ANALOG
#define LL_GPIO_MODE_ANALOG       GPIO_MODE_ANALOG
#endif
#ifndef LL_GPIO_PULL_DOWN
#define LL_GPIO_PULL_DOWN         GPIO_PULL_DOWN
#endif
#ifndef LL_GPIO_PULL_UP
#define LL_GPIO_PULL_UP           GPIO_PULL_UP
#endif
#ifndef LL_GPIO_PULL_NO
#define LL_GPIO_PULL_NO           GPIO_PULL_NONE
#endif

#ifndef LL_GPIO_PIN_0
#define LL_GPIO_PIN_0     GPIO_PINS_0
#define LL_GPIO_PIN_1     GPIO_PINS_1
#define LL_GPIO_PIN_2     GPIO_PINS_2
#define LL_GPIO_PIN_3     GPIO_PINS_3
#define LL_GPIO_PIN_4     GPIO_PINS_4
#define LL_GPIO_PIN_5     GPIO_PINS_5
#define LL_GPIO_PIN_6     GPIO_PINS_6
#define LL_GPIO_PIN_7     GPIO_PINS_7
#define LL_GPIO_PIN_8     GPIO_PINS_8
#define LL_GPIO_PIN_9     GPIO_PINS_9
#define LL_GPIO_PIN_10    GPIO_PINS_10
#define LL_GPIO_PIN_11    GPIO_PINS_11
#define LL_GPIO_PIN_12    GPIO_PINS_12
#define LL_GPIO_PIN_13    GPIO_PINS_13
#define LL_GPIO_PIN_14    GPIO_PINS_14
#define LL_GPIO_PIN_15    GPIO_PINS_15
#endif

#ifndef LL_TIM_IsActiveFlag_UPDATE
#define LL_TIM_IsActiveFlag_UPDATE(x)    tmr_flag_get((x), TMR_OVF_FLAG)
#endif
#ifndef LL_TIM_IsEnabledIT_UPDATE
#define LL_TIM_IsEnabledIT_UPDATE(x)     (((x)->iden & (TMR_OVF_INT)) != 0)
#endif
#ifndef LL_TIM_ClearFlag_UPDATE
#define LL_TIM_ClearFlag_UPDATE(x)       tmr_flag_clear((x), TMR_OVF_FLAG)
#endif
#ifndef LL_TIM_EnableIT_UPDATE
#define LL_TIM_EnableIT_UPDATE(x)        tmr_interrupt_enable((x), TMR_OVF_INT, TRUE)
#endif
#ifndef LL_TIM_DisableIT_UPDATE
#define LL_TIM_DisableIT_UPDATE(x)       tmr_interrupt_enable((x), TMR_OVF_INT, FALSE)
#endif
#ifndef LL_TIM_IsActiveFlag_CC1
#define LL_TIM_IsActiveFlag_CC1(x)       tmr_flag_get((x), TMR_C1_FLAG)
#endif
#ifndef LL_TIM_IsEnabledIT_CC1
#define LL_TIM_IsEnabledIT_CC1(x)        (((x)->iden & (TMR_C1_INT)) != 0)
#endif
#ifndef LL_TIM_ClearFlag_CC1
#define LL_TIM_ClearFlag_CC1(x)          tmr_flag_clear((x), TMR_C1_FLAG)
#endif
#ifndef LL_TIM_EnableIT_CC1
#define LL_TIM_EnableIT_CC1(x)           tmr_interrupt_enable((x), TMR_C1_INT, TRUE)
#endif
#ifndef LL_TIM_DisableIT_CC1
#define LL_TIM_DisableIT_CC1(x)          tmr_interrupt_enable((x), TMR_C1_INT, FALSE)
#endif
#ifndef LL_TIM_IsActiveFlag_CC2
#define LL_TIM_IsActiveFlag_CC2(x)       tmr_flag_get((x), TMR_C2_FLAG)
#endif
#ifndef LL_TIM_IsEnabledIT_CC2
#define LL_TIM_IsEnabledIT_CC2(x)        (((x)->iden & (TMR_C2_INT)) != 0)
#endif
#ifndef LL_TIM_ClearFlag_CC2
#define LL_TIM_ClearFlag_CC2(x)          tmr_flag_clear((x), TMR_C2_FLAG)
#endif
#ifndef LL_TIM_EnableIT_CC2
#define LL_TIM_EnableIT_CC2(x)           tmr_interrupt_enable((x), TMR_C2_INT, TRUE)
#endif
#ifndef LL_TIM_DisableIT_CC2
#define LL_TIM_DisableIT_CC2(x)          tmr_interrupt_enable((x), TMR_C2_INT, FALSE)
#endif

#ifndef LL_TIM_EnableCounter
#define LL_TIM_EnableCounter(x)          tmr_counter_enable((x), TRUE);
#endif
#ifndef LL_TIM_DisableCounter
#define LL_TIM_DisableCounter(x)         tmr_counter_enable((x), FALSE);
#endif
#ifndef LL_TIM_GetCounter
#define LL_TIM_GetCounter                tmr_counter_value_get
#endif
#ifndef LL_TIM_SetCounter
#define LL_TIM_SetCounter                tmr_counter_value_set
#endif

#ifndef CCR1
#define CCR1 c1dt
#endif
#ifndef CCR2
#define CCR2 c2dt
#endif
#ifndef SMCR
#define SMCR stctrl
#endif
#ifndef CCER
#define CCER cctrl
#endif
#ifndef EGR
#define EGR swevt
#endif
#ifndef TIM_EGR_UG
#define TIM_EGR_UG    (1 << 0)
#endif
#ifndef TIM_CCER_CC1E
#define TIM_CCER_CC1E (1 << 0)
#endif

#ifndef __LL_TIM_CALC_PSC
#define __LL_TIM_CALC_PSC(__TIMCLK__, __CNTCLK__)   \
  (((__TIMCLK__) >= (__CNTCLK__)) ? (uint32_t)(((__TIMCLK__)/(__CNTCLK__)) - 1U) : 0U)
#endif

#ifndef LL_DMA_CHANNEL_3
#define LL_DMA_CHANNEL_3    DMA1_CHANNEL3
#endif
#ifndef LL_DMA_CHANNEL_5
#define LL_DMA_CHANNEL_5    DMA1_CHANNEL5
#endif

#ifndef LL_USART_IsActiveFlag_TXE
#define LL_USART_IsActiveFlag_TXE(x)     usart_flag_get((x), USART_TDBE_FLAG)
#endif
#ifndef LL_USART_IsEnabledIT_TXE
#define LL_USART_IsEnabledIT_TXE(x)      ((x)->ctrl1_bit.tdbeien != 0)
#endif
#ifndef LL_USART_ClearFlag_TXE
#define LL_USART_ClearFlag_TXE(x)        usart_flag_clear((x), USART_TDBE_FLAG)
#endif
#ifndef LL_USART_EnableIT_TXE
#define LL_USART_EnableIT_TXE(x)         usart_interrupt_enable((x), USART_TDBE_INT)
#endif
#ifndef LL_USART_IsActiveFlag_TC
#define LL_USART_IsActiveFlag_TC(x)      usart_flag_get((x), USART_TDC_FLAG)
#endif
#ifndef LL_USART_IsEnabledIT_TC
#define LL_USART_IsEnabledIT_TC(x)       ((x)->ctrl1_bit.tdcien != 0)
#endif
#ifndef LL_USART_ClearFlag_TC
#define LL_USART_ClearFlag_TC(x)         usart_flag_clear((x), USART_TDC_FLAG)
#endif
#ifndef LL_USART_EnableIT_TC
#define LL_USART_EnableIT_TC(x)          usart_interrupt_enable((x), USART_TDC_INT)
#endif
#ifndef LL_USART_IsActiveFlag_RXNE
#define LL_USART_IsActiveFlag_RXNE(x)     usart_flag_get((x), USART_RDBF_FLAG)
#endif
#ifndef LL_USART_IsEnabledIT_RXNE
#define LL_USART_IsEnabledIT_RXNE(x)      ((x)->ctrl1_bit.rdbfien != 0)
#endif
#ifndef LL_USART_ClearFlag_RXNE
#define LL_USART_ClearFlag_RXNE(x)        usart_flag_clear((x), USART_RDBF_FLAG)
#endif
#ifndef LL_USART_EnableIT_RXNE
#define LL_USART_EnableIT_RXNE(x)         usart_interrupt_enable((x), USART_RDBF_INT)
#endif
#ifndef LL_USART_IsActiveFlag_IDLE
#define LL_USART_IsActiveFlag_IDLE(x)     usart_flag_get((x), USART_IDLEF_FLAG)
#endif
#ifndef LL_USART_IsEnabledIT_IDLE
#define LL_USART_IsEnabledIT_IDLE(x)      ((x)->ctrl1_bit.idleien != 0)
#endif
#ifndef LL_USART_ClearFlag_IDLE
#define LL_USART_ClearFlag_IDLE(x)        usart_flag_clear((x), USART_IDLEF_FLAG)
#endif
#ifndef LL_USART_EnableIT_IDLE
#define LL_USART_EnableIT_IDLE(x)         usart_interrupt_enable((x), USART_IDLE_INT)
#endif
#ifndef LL_USART_TransmitData8
#define LL_USART_TransmitData8            usart_data_transmit
#endif
#ifndef LL_USART_ReceiveData8
#define LL_USART_ReceiveData8             usart_data_receive
#endif
#ifndef LL_USART_Enable
#define LL_USART_Enable(x)                usart_enable((x), TRUE)
#endif

#ifndef LL_DMA_DisableChannel
#define LL_DMA_DisableChannel(x, y)       dma_channel_enable((y), FALSE)
#endif
#ifndef LL_DMA_EnableChannel
#define LL_DMA_EnableChannel(x, y)        dma_channel_enable((y), TRUE)
#endif
#ifndef LL_DMA_SetDataLength
#define LL_DMA_SetDataLength(x, y, z)     dma_data_number_set((y), (z))
#endif
#ifndef LL_USART_EnableDMAReq_RX
#define LL_USART_EnableDMAReq_RX(x)       usart_dma_receiver_enable((x), TRUE)
#endif
#ifndef LL_USART_DisableDMAReq_RX
#define LL_USART_DisableDMAReq_RX(x)      usart_dma_receiver_enable((x), FALSE)
#endif

#ifdef __cplusplus
}
#endif

#endif
