#pragma once

#ifdef STMICRO
#include "stm32.h"

#define CLK_CNT(rate) ((SystemCoreClock + ((rate) >> 1)) / (rate))

#ifndef TIM_SMCR_SMS_RM
#define TIM_SMCR_SMS_RM    (TIM_SMCR_SMS_2)
#endif

#ifndef TIM_SMCR_TS_TI1FP1
#define TIM_SMCR_TS_TI1FP1    (TIM_SMCR_TS_0 | TIM_SMCR_TS_2)
#endif

#endif

void mcu_init(void);
