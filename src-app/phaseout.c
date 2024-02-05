#include "phaseout.h"

#include "userconfig.h"

#ifndef HW_PHASES_REMAP
#define HW_PHASES_REMAP 0
#endif

enum
{
    PWMPINSTATE_LOW,
    PWMPINSTATE_PWM,
    PWMPINSTATE_FLT,
    PWMPINSTATE_PWMOD,
    PWMPINSTATE_PWMPUSH,
};

bool braking;
bool complementary_pwm;
static uint8_t phase_remap = 0;
static bool load_balance = false;
static bool open_drain = false;

static uint8_t all_pin_states;

void pwm_all_flt()
{
    pwm_setFlt_A();
    pwm_setFlt_B();
    pwm_setFlt_C();
    all_pin_states = PWMPINSTATE_FLT;
    pwm_setDuty_A(0); pwm_setDuty_B(0); pwm_setDuty_C(0);
}

void pwm_all_low()
{
    pwm_setLow_A();
    pwm_setLow_B();
    pwm_setLow_C();
    all_pin_states = PWMPINSTATE_LOW;
    pwm_setDuty_A(0); pwm_setDuty_B(0); pwm_setDuty_C(0);
}

void pwm_all_pwm()
{
    pwm_setPWM_A();
    pwm_setPWM_B();
    pwm_setPWM_C();
    all_pin_states = PWMPINSTATE_PWM;
}

void pwm_all_pwmod()
{
    pwm_setODPWM_A();
    pwm_setODPWM_B();
    pwm_setODPWM_C();
    all_pin_states = PWMPINSTATE_PWMOD;
}

void pwm_all_pwmpush()
{
    pwm_setHIPWM_A();
    pwm_setHIPWM_B();
    pwm_setHIPWM_C();
    all_pin_states = PWMPINSTATE_PWMPUSH;
}

void pwm_full_brake()
{
    pwm_set_braking(true);
    pwm_all_low();
}

void pwm_full_coast()
{
    pwm_set_braking(false);
    pwm_all_flt();
}

void pwm_set_all_duty(uint16_t a, uint16_t b, uint16_t c)
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

void pwm_set_all_duty_remapped(uint16_t a, uint16_t b, uint16_t c)
{
    if (load_balance && open_drain == false)
    {
        // tone down the power if one of the common-shared MOSFETs will take more power than what any of the other MOSFETs could possibly ever take

        uint16_t max_duty = cfg.pwm_period;
        uint16_t mid_duty = (max_duty + 1) / 2;
        int p1, p2;
        if (b >= a && c >= a)
        {
            p1 = b - a;
            p2 = c - a;
        }
        else if (b <= a && c <= a)
        {
            p1 = a - b;
            p2 = a - c;
        }
        int total_power = p1 + p2;
        int tries = 3;
        while (total_power > mid_duty && tries--)
        {
            p1 = fi_map(p1, 0, total_power, 0, mid_duty, false);
            p2 = fi_map(p2, 0, total_power, 0, mid_duty, false);
            total_power = p1 + p2;
        }
        if (b >= a && c >= a)
        {
            b = a + p1;
            c = a + p2;
        }
        else if (b <= a && c <= a)
        {
            b = a - p1;
            c = a - p2;
        }
    }

    if (braking && open_drain == false && a == b && a == c)
    {
        // math calculated stop, so actually feed no voltage
        a = 0;
        b = 0;
        c = 0;
    }

    switch (phase_remap)
    {
        case 0: pwm_set_all_duty(a, b, c); break;
        case 1: pwm_set_all_duty(b, a, c); break;
        case 2: pwm_set_all_duty(c, a, b); break;
        // the cases below can also be handled by swapping the channels, so they are useless
        // case 3: pwm_set_all_duty(a, c, b); break;
        // case 4: pwm_set_all_duty(b, c, a); break;
        // case 5: pwm_set_all_duty(c, b, a); break;
    }
}

void pwm_set_remap(uint8_t map)
{
    // recalculate to 0 index
    phase_remap = map - ((map >= 1) ? 1 : 0);
    phase_remap += HW_PHASES_REMAP; // change the mapping by compile time definition
    phase_remap %= 3;
}

void pwm_set_loadbalance(bool x)
{
    load_balance = x;
}

void pwm_set_braking(bool x)
{
    bool set = (x != complementary_pwm && all_pin_states == PWMPINSTATE_PWM);
    braking = x;
    complementary_pwm = x;
    if (set) {
        pwm_all_pwm();
    }
}

#ifdef DEBUG_PRINT

void pwm_debug_report(void) {
    dbg_printf("%u, %u, %u, ", pwm_getDuty_A(), pwm_getDuty_B(), pwm_getDuty_C());
}

#endif
