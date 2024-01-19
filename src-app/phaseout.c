#include "phaseout.h"

bool braking;

enum
{
    PWMPINSTATE_LOW,
    PWMPINSTATE_PWM,
    PWMPINSTATE_FLT,
};

static uint8_t all_pin_states;

void pwm_all_flt()
{
    pwm_setFlt_A();
    pwm_setFlt_B();
    pwm_setFlt_C();
    all_pin_states = PWMPINSTATE_FLT;
}

void pwm_all_low()
{
    pwm_setLow_A();
    pwm_setLow_B();
    pwm_setLow_C();
    all_pin_states = PWMPINSTATE_LOW;
}

void pwm_all_pwm()
{
    pwm_setPWM_A();
    pwm_setPWM_B();
    pwm_setPWM_C();
    all_pin_states = PWMPINSTATE_PWM;
}

void pwm_full_brake()
{
    braking = true;
    pwm_all_low();
}

void pwm_full_coast()
{
    braking = false;
    pwm_all_flt();
}

void pwm_set_all_duty(uint16_t a, uint16_t b, uint16_t c);
{
    // this logic only applies to brushed motor operation
    // do not use this code for brushless motors
    if (a != 0 || b != 0 || c != 0) {
        // automatically exit out of braking or coasting state
        if (all_pin_states != PWMPINSTATE_PWM) {
            pwm_all_pwm();
        }
    }
    else if (a == 0 && b == 0 && c == 0) {
        // automatically brake or coast depending on setting
        if (braking) {
            pwm_full_brake();
        }
        else {
            pwm_full_coast();
        }
        // all_pin_states automatically updated already
    }

    pwm_setDuty_A(a);
    pwm_setDuty_B(b);
    pwm_setDuty_C(c);
}

#ifndef HW_PHASES_REMAP
#define HW_PHASES_REMAP 0
#endif

void pwm_set_all_duty_remapped(uint16_t a, uint16_t b, uint16_t c, uint8_t map)
{
    map += HW_PHASES_REMAP;
    map %= 6;
    switch (map)
    {
        case 0: pwm_set_all_duty(a, b, c); break;
        case 1: pwm_set_all_duty(a, c, b); break;
        case 2: pwm_set_all_duty(b, a, c); break;
        case 3: pwm_set_all_duty(b, c, a); break;
        case 4: pwm_set_all_duty(c, a, b); break;
        case 5: pwm_set_all_duty(c, b, a); break;
    }
}
