#pragma once

#include "mcu.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool braking;

extern void pwm_init();        // implemented in HAL
extern void pwm_set_period(uint32_t x);   // used to set frequency from user config

// implemented in HAL
extern void pwm_setFlt_A();
extern void pwm_setLow_A();
extern void pwm_setFlt_B();
extern void pwm_setLow_B();
extern void pwm_setFlt_C();
extern void pwm_setLow_C();

extern void pwm_setPWM_CP_A();
extern void pwm_setPWM_CP_B();
extern void pwm_setPWM_CP_C();
extern void pwm_setPWM_OD_A();
extern void pwm_setPWM_OD_B();
extern void pwm_setPWM_OD_C();
extern void pwm_setPWM_HI_A();
extern void pwm_setPWM_HI_B();
extern void pwm_setPWM_HI_C();

// wrappers
void pwm_all_flt();
void pwm_all_low();

void pwm_set_all_duty(uint16_t a, uint8_t ma, uint16_t b, uint8_t mb, uint16_t c, uint8_t mc); // directly writes to each phase, and set the phase pin mode, without remap
void pwm_set_all_duty_remapped(uint16_t p_common, uint16_t p_left, uint16_t p_right); // accounts for remapping set by pwm_set_remap

#ifdef DEBUG_PRINT
void pwm_debug_report(void);
void pwm_debug_report_drive(void);
#else
#define pwm_debug_report(...)
#define pwm_debug_report_drive(...)
#endif

void pwm_set_remap(uint8_t);    // set from application
void pwm_set_loadbalance(bool); // set from application
void pwm_set_braking(bool);     // set from application

#ifdef __cplusplus
}
#endif

#include "mcu.h"

#if defined(STMICRO)
#include "phaseout_stm32.h"
#elif defined(ARTERY)
#include "phaseout_at32.h"
#endif
