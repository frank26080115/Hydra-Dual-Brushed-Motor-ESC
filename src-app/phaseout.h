#pragma once

#include "mcu.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool complementary_pwm; // this project will always have this true
extern bool braking;           // set by user

extern void pwm_init();        // implemented in HAL

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

// wrappers
void pwm_all_flt();
void pwm_all_low();
void pwm_all_pwm();

void pwm_proportional_brake(); // implement this in HAL
void pwm_full_brake();
void pwm_full_coast();

void pwm_set_all_duty(uint16_t a, uint16_t b, uint16_t c);          // directly writes to each phase without remap
void pwm_set_all_duty_remapped(uint16_t a, uint16_t b, uint16_t c); // accounts for remapping set by pwm_set_remap
// note: in this project, I'm using phase A as the common-shared phase between the two motors, b is channel 1, c is channel 2

void pwm_set_remap(uint8_t);

#ifdef __cplusplus
}
#endif
