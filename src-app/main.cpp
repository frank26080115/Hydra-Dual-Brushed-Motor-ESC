#include "main.h"
#include "userconfig.h"
#include "inputpin.h"

int main(void)
{
    mcu_init();
    led_init();
    inp_init();
    pwm_init();
    pwm_all_flt();

    eeprom_load_or_default();

    boot_decide_cli();
    ledblink_disarmed();

    while (true)
    {

    }

    return 0;
}

void boot_decide_cli(void)
{
    ledblink_boot();

    if (inp_read() == 0) {
        while (inp_read() == 0) {
            led_task();
            if (millis() >= CLI_ENTER_LOW_CRITERIA) {
                inp_pullUp();
                ledblink_boot2();
                break;
            }
        }
        while (inp_read() == 0) {
            led_task();
        }
    }

    if (millis() < CLI_ENTER_LOW_CRITERIA) {
        // did not remain unplugged long enough
        // do not enter CLI
        return;
    }

    inp_pullDown();

    uint32_t t = millis();
    uint8_t low_pulse_cnt = 0;
    while (true) {
        led_task();
        // debounce low pulses from hot-plug
        if (inp_read() == 0) {
            // measure the low pulse
            uint32_t t2 = millis();
            while (inp_read() == 0) {
                led_task();
            }
            uint32_t t3 = millis();
            if ((t3 - t2 >= 5)) {
                // low pulse too long, it's probably a RC pulse
                // do not enter CLI
                return;
            }
            else {
                // low pulse very short
                low_pulse_cnt++;
                if (low_pulse_cnt >= 10) {
                    // too many of these low pulses
                    // do not enter CLI
                    return;
                }
            }
        }
        if ((millis() - t) >= CLI_ENTER_HIGH_CRITERIA) {
            // has been long enough
            ledblink_cli();
            cli_enter(); // this never returns
        }
    }
}