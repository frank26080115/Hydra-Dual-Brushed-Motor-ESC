#pragma once

#include "mcu.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool complementry_pwm;
extern bool braking;

extern void pwm_init();

extern void pwm_setPWM_A();
extern void pwm_setFlt_A();
extern void pwm_setLow_A();
extern void pwm_setPWM_B();
extern void pwm_setFlt_B();
extern void pwm_setLow_B();
extern void pwm_setPWM_C();
extern void pwm_setFlt_C();
extern void pwm_setLow_C();

void pwm_all_flt();
void pwm_all_low();
void pwm_all_pwm();

extern inline void pwm_setDuty_A(uint16_t x);
extern inline void pwm_setDuty_B(uint16_t x);
extern inline void pwm_setDuty_C(uint16_t x);

void pwm_proportional_brake(); // implement this in HAL
void pwm_full_brake();
void pwm_full_coast();
void pwm_set_all_duty(uint16_t a, uint16_t b, uint16_t c);
void pwm_set_all_duty_remapped(uint16_t a, uint16_t b, uint16_t c, uint8_t map);

#ifdef __cplusplus
}
#endif
