#pragma once

#include "mcu.h"
#include "phaseout.h"

#define PWMOUTTIMx TMR1

#ifdef __cplusplus
extern "C" {
#endif

static inline void pwm_setDuty_A(uint16_t x)
{
    PWMOUTTIMx->c1dt = x;
}

static inline void pwm_setDuty_B(uint16_t x)
{
    PWMOUTTIMx->c2dt = x;
}

static inline void pwm_setDuty_C(uint16_t x)
{
    PWMOUTTIMx->c3dt = x;
}

static inline uint16_t pwm_getDuty_A(void)
{
    return PWMOUTTIMx->c3dt;
}

static inline uint16_t pwm_getDuty_B(void)
{
    return PWMOUTTIMx->c2dt;
}

static inline uint16_t pwm_getDuty_C(void)
{
    return PWMOUTTIMx->c1dt;
}

#ifdef __cplusplus
}
#endif
