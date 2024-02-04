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

#ifdef __cplusplus
}
#endif
