#pragma once

#include "main.h"
#include "mcu.h"

void hwtest_led(void);
void hwtest_pwm(void);
void hwtest_adc(void);
void hwtest_rc1(void);
void hwtest_rc2(void);
void hwtest_rc_crsf(void);
void hwtest_eeprom(void);
void hwtest_sense(void);
void hwtest_uart(void);
void hwtest_cli(void);

void hwtest_gpio(GPIO_TypeDef* gpio, uint32_t pin);

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
