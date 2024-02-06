#pragma once

#include "main.h"
#include "mcu.h"

void hwtest_led(void);
void hwtest_pwm(void);
void hwtest_pwm_max(void);
void hwtest_phases(void);
void hwtest_adc(void);
void hwtest_rc1(void);
void hwtest_rc2(void);
void hwtest_rc12(void);
void hwtest_rc_crsf(void);
void hwtest_eeprom(void);
void hwtest_sense(void);
void hwtest_uart(void);
void hwtest_uart1(void);
void hwtest_uart2(void);
void hwtest_uart_swc(void);
void hwtest_cli(void);
void hwtest_tone(void);
void hwtest_rc_led_pwm(void);
void hwtest_rc_led_crsf(void);
void hwtest_rc_tone_pwm(void);
void hwtest_rc_tone_crsf(void);
void hwtest_simCurrentLimit(void);

void hwtest_gpio(
    #if defined(STMICRO)
        GPIO_TypeDef
    #elif defined(ARTERY)
        gpio_type
    #endif
        * gpio, uint32_t pin);

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
