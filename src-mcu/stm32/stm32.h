#pragma once

#include "targets.h"

#ifdef MCU_F051
#include "stm32f051x8.h"
#include "stm32f0xx.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_adc.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_rcc.h"
#endif

#ifdef MCU_F031
// TODO
#endif

#ifdef MCU_G071
#include "stm32g071xx.h"
#include "stm32g0xx.h"
#include "system_stm32g0xx.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_rcc.h"
#endif

#define GPIO_PIN_SWDIO LL_GPIO_PIN_13
#define GPIO_PIN_SWCLK LL_GPIO_PIN_14

#define is_in_isr()    ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0)

#define CLK_CNT(rate) ((SystemCoreClock + ((rate) >> 1)) / (rate))

#ifndef TIM_SMCR_SMS_RM
#define TIM_SMCR_SMS_RM    (TIM_SMCR_SMS_2)
#endif

#ifndef TIM_SMCR_TS_TI1F_ED
#define TIM_SMCR_TS_TI1F_ED    (TIM_SMCR_TS_2)
#endif

#ifndef TIM_SMCR_TS_TI1FP1
#define TIM_SMCR_TS_TI1FP1    (TIM_SMCR_TS_0 | TIM_SMCR_TS_2)
#endif

#ifndef TIM_CCMR1_CC1S_IN_TI1
#define TIM_CCMR1_CC1S_IN_TI1    (TIM_CCMR1_CC1S_0)
#endif
#ifndef TIM_CCMR1_CC2S_IN_TI1
#define TIM_CCMR1_CC2S_IN_TI1    (TIM_CCMR1_CC2S_1)
#endif

#ifndef TIM_CCMR1_IC1F_CK_INT_N_8
#define TIM_CCMR1_IC1F_CK_INT_N_8    (TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1)
#define TIM_CCMR1_IC2F_CK_INT_N_8    (TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_1)
#define TIM_CCMR1_IC3F_CK_INT_N_8    (TIM_CCMR1_IC3F_0 | TIM_CCMR1_IC3F_1)
#define TIM_CCMR1_IC4F_CK_INT_N_8    (TIM_CCMR1_IC4F_0 | TIM_CCMR1_IC4F_1)
#endif

#ifndef TIM_CCMR1_IC1F_DTF_DIV_8_N_8
#define TIM_CCMR1_IC1F_DTF_DIV_8_N_8    (TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_3)
#define TIM_CCMR1_IC2F_DTF_DIV_8_N_8    (TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_3)
#define TIM_CCMR1_IC3F_DTF_DIV_8_N_8    (TIM_CCMR1_IC3F_0 | TIM_CCMR1_IC3F_3)
#define TIM_CCMR1_IC4F_DTF_DIV_8_N_8    (TIM_CCMR1_IC4F_0 | TIM_CCMR1_IC4F_3)
#endif

#ifndef TIM_CCMR1_OC1M_PWM2
#define TIM_CCMR1_OC1M_PWM2    (TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2)
#endif
