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

static inline uint16_t pwm_getDuty_A(void)
{
    return PWMOUTTIMx->CCR3;
}

static inline uint16_t pwm_getDuty_B(void)
{
    return PWMOUTTIMx->CCR2;
}

static inline uint16_t pwm_getDuty_C(void)
{
    return PWMOUTTIMx->CCR1;
}

#ifdef __cplusplus
}
#endif
