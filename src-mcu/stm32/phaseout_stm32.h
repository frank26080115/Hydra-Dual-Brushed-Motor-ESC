#pragma once

#include "mcu.h"
#include "phaseout.h"

#define PWMOUTTIMx TIM1

#ifdef __cplusplus
extern "C" {
#endif

static inline void pwm_setDuty_A(uint16_t x)
{
    PWMOUTTIMx->CCR3 = x;
}

static inline void pwm_setDuty_B(uint16_t x)
{
    PWMOUTTIMx->CCR2 = x;
}

static inline void pwm_setDuty_C(uint16_t x)
{
    PWMOUTTIMx->CCR1 = x;
}

#ifdef __cplusplus
}
#endif
