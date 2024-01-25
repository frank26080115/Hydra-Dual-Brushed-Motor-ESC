#include "main.h"
#include "hw_tests.h"
#include "debug_tools.h"
#include "phaseout.h"
#include "sense.h"

#ifdef STMICRO
#include "rc_stm32.h"
#include "cereal_timer.h"
#include "cereal_usart.h"
#include "swd_pins.h"
#endif

#ifdef DEVELOPMENT_BOARD
extern Cereal_USART dbg_cer;
#endif

#ifdef STMICRO
extern RcPulse_InputCap rc_pulse_1;
#endif

#ifdef HW_TESTS
void hw_test(void)
{
    //hwtest_adc();
    //hwtest_pwm();
    hwtest_rc1();
}
#endif

void hwtest_pwm(void)
{
    dbg_button_init();
    pwm_init();
    dbg_switch_to_pwm();
    pwm_set_braking(true);
    pwm_full_coast();
    pwm_set_reload(2000);
    pwm_set_remap(1);
    pwm_set_loadbalance(false);
    while (true)
    {
        if (dbg_read_btn())
        {
            uint32_t x = (millis() / 10) % 1900;
            pwm_set_all_duty_remapped(x, x + 1, x + 2);
        }
        else
        {
            pwm_set_all_duty_remapped(0, 0, 0);
        }
    }
}

void hwtest_adc(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
    #endif
    sense_init();
    uint32_t t = millis();
    while (true)
    {
        sense_task();
        if ((millis() - t) >= 200)
        {
            t = millis();
            dbg_printf("[%u]  v = %u     c = %u    t = %u\r\n", t, adc_raw_voltage, adc_raw_current, adc_raw_temperature);
        }
    }
}

void hwtest_rc1(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
    #endif
    rc_pulse_1.init();
    uint32_t t = millis();
    while (true)
    {
        rc_pulse_1.task();
        if ((millis() - t) >= 200)
        {
            t = millis();
            if (rc_pulse_1.is_alive()) {
                dbg_printf("[%u]  RC1 = %u\r\n", t, rc_pulse_1.readRaw());
            }
            else {
                dbg_printf("[%u]  RC1 = ?\r\n", t);
            }
        }
    }
}
