#pragma once

#include "mcu.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool braking;
extern bool complementary_pwm;

extern void pwm_init();        // implemented in HAL
extern void pwm_set_period(uint32_t x);   // used to set frequency from user config
extern void pwm_set_deadtime(uint32_t x); // used to set deadtime from user config

// implemented in HAL
extern void pwm_setPWM_A();
extern void pwm_setFlt_A();
extern void pwm_setLow_A();
extern void pwm_setPWM_B();
extern void pwm_setFlt_B();
extern void pwm_setLow_B();
extern void pwm_setPWM_C();
extern void pwm_setFlt_C();
extern void pwm_setLow_C();

extern void pwm_setODPWM_A();
extern void pwm_setODPWM_B();
extern void pwm_setODPWM_C();
extern void pwm_setHIPWM_A();
extern void pwm_setHIPWM_B();
extern void pwm_setHIPWM_C();

// wrappers
void pwm_all_flt();
void pwm_all_low();
void pwm_all_pwm();

void pwm_full_brake();
void pwm_full_coast();

void pwm_set_all_duty(uint16_t a, uint16_t b, uint16_t c);          // directly writes to each phase without remap
void pwm_set_all_duty_remapped(uint16_t a, uint16_t b, uint16_t c); // accounts for remapping set by pwm_set_remap
// note: in this project, I'm using phase A as the common-shared phase between the two motors, b is channel 1, c is channel 2

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
