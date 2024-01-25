#pragma once

#include "main.h"

void hwtest_led(void);
void hwtest_pwm(void);
void hwtest_adc(void);
void hwtest_rc1(void);
void hwtest_rc2(void);
void hwtest_rc_crsf(void);
void hwtest_eeprom(void);

#ifdef ENABLE_COMPILE_CLI
void hwtest_bbcer(void);
#else
#define hwtest_bbcer(...)
#endif

#ifndef HW_TESTS
#define hw_test(...)
#else
void hw_test(void);
#endif
