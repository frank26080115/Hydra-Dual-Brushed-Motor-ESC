#pragma once

#include "main.h"

void hwtest_pwm(void);
void hwtest_adc(void);
void hwtest_rc1(void);
void hwtest_rc2(void);

#ifndef HW_TESTS
#define hw_test(...)
#else
void hw_test(void);
#endif

