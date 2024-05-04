#include "phaseout.h"

#include "userconfig.h"

#ifndef HW_PHASES_REMAP
#define HW_PHASES_REMAP 0
#endif

enum
{
    PWMPINSTATE_PWM_COMP,
    PWMPINSTATE_PWM_OD,
    PWMPINSTATE_PWM_HI,
    PWMPINSTATE_FLT,
};

bool braking;
static uint8_t phase_remap = 0;
static bool load_balance = false;

#ifdef DEBUG_PRINT
int16_t dbg_drv_left;
int16_t dbg_drv_right;
#endif

void pwm_all_flt()
{
    pwm_setFlt_A();
    pwm_setFlt_B();
    pwm_setFlt_C();
    pwm_setDuty_A(0); pwm_setDuty_B(0); pwm_setDuty_C(0);
}

void pwm_all_low()
{
    pwm_setLow_A();
    pwm_setLow_B();
    pwm_setLow_C();
    pwm_setDuty_A(0); pwm_setDuty_B(0); pwm_setDuty_C(0);
}

void pwm_set_all_duty(uint16_t a, uint8_t ma, uint16_t b, uint8_t mb, uint16_t c, uint8_t mc)
{
    #define PWM_PIN_SET(lt, mx, pw)     do { if ((mx) == PWMPINSTATE_PWM_COMP) { pwm_setPWM_CP_ ##lt (); } \
                                        else if ((mx) == PWMPINSTATE_PWM_OD)   { pwm_setPWM_OD_ ##lt (); } \
                                        else if ((mx) == PWMPINSTATE_PWM_HI)   { pwm_setPWM_HI_ ##lt (); } \
                                        else { pwm_setFlt_ ##lt (); } pwm_setDuty_ ##lt (pw); } while (0) \

    PWM_PIN_SET(A, ma, a);
    PWM_PIN_SET(B, mb, b);
    PWM_PIN_SET(C, mc, c);
}

void pwm_set_all_duty_remapped(uint16_t p_common, uint16_t p_left, uint16_t p_right)
{
    int p1 = 0, p2 = 0;
    if (load_balance)
    {
        // tone down the power if one of the common-shared MOSFETs will take more power than what any of the other MOSFETs could possibly ever take

        uint16_t max_duty = cfg.pwm_period - cfg.pwm_deadtime;
        uint16_t mid_duty = (max_duty + 1) / 2;
        if (p_left >= p_common && p_right >= p_common)
        {
            p1 = p_left  - p_common;
            p2 = p_right - p_common;
        }
        else if (p_left <= p_common && p_right <= p_common)
        {
            p1 = p_common - p_left;
            p2 = p_common - p_right;
        }
        else
        {
            // do nothing, load is being shared by both high-side and low-side MOSFETs
        }
        int total_power = p1 + p2;
        int tries = 3;
        while (total_power > mid_duty && tries--)
        {
            p1 = fi_map(p1, 0, total_power, 0, mid_duty, false);
            p2 = fi_map(p2, 0, total_power, 0, mid_duty, false);
            total_power = p1 + p2;
        }
        if (p_left >= p_common && p_right >= p_common)
        {
            p_left  = p_common + p1;
            p_right = p_common + p2;
        }
        else if (p_left <= p_common && p_right <= p_common)
        {
            p_left  = p_common - p1;
            p_right = p_common - p2;
        }
    }

    if (braking && p_common == p_left && p_common == p_right)
    {
        // math calculated stop, so actually feed no voltage
        p_common = 0;
        p_left   = 0;
        p_right  = 0;
    }

    p1 = p_left - p_common, p2 = p_right - p_common;

    #ifdef DEBUG_PRINT
    dbg_drv_left  = p1;
    dbg_drv_right = p2;
    #endif

    uint16_t a = p_common, b = p_left, c = p_right;
    uint8_t ma = braking ? PWMPINSTATE_PWM_COMP : (p_common == 0 && p_left == 0 && p_right == 0 ? PWMPINSTATE_FLT : PWMPINSTATE_PWM_COMP);
    uint8_t mb = braking ? PWMPINSTATE_PWM_COMP : (p1 > 0 ? PWMPINSTATE_PWM_HI : (p1 < 0 ? PWMPINSTATE_PWM_OD : PWMPINSTATE_FLT));
    uint8_t mc = braking ? PWMPINSTATE_PWM_COMP : (p2 > 0 ? PWMPINSTATE_PWM_HI : (p2 < 0 ? PWMPINSTATE_PWM_OD : PWMPINSTATE_FLT));

    switch (phase_remap)
    {
        case 0: pwm_set_all_duty(a, ma, b, mb, c, mc); break;
        case 1: pwm_set_all_duty(b, mb, a, ma, c, mc); break;
        case 2: pwm_set_all_duty(c, mc, a, ma, b, mb); break;
        // the cases below can also be handled by swapping the channels, so they are useless
        // case 3: pwm_set_all_duty(a, ma, c, mc, b, mb); break;
        // case 4: pwm_set_all_duty(b, mb, c, mc, a, ma); break;
        // case 5: pwm_set_all_duty(c, mc, b, mb, a, ma); break;
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
    braking = x;
    // note: won't actually do anything until pwm_set_all_duty_remapped is called
}

#ifdef DEBUG_PRINT

void pwm_debug_report(void) {
    dbg_printf("%u, %u, %u, ", pwm_getDuty_A(), pwm_getDuty_B(), pwm_getDuty_C());
}

void pwm_debug_report_drive(void) {
    dbg_printf("%d, %d, ", dbg_drv_left, dbg_drv_right);
}

#endif
