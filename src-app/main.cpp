#include "main.h"
#include "userconfig.h"
#include "version.h"
#include "inputpin.h"
#include "phaseout.h"
#include "sense.h"
#include "rc.h"
#include "led.h"
#include "crsf.h"
#include <math.h>

#ifdef STMICRO
#include "rc_stm32.h"
#include "cereal_timer.h"
#include "cereal_usart.h"
#endif

RcChannel* rc1;
RcChannel* rc2;

#if defined(STM32F051DISCO) || defined(STM32G071NUCLEO)
Cereal_USART dbg_cer(3);
#endif

#ifdef STMICRO
RcPulse_InputCap rc_pulse_1(IC_TIMER_REGISTER, INPUT_PIN_PORT, INPUT_PIN, IC_TIMER_CHANNEL);
RcPulse_GpioIsr  rc_pulse_2(GPIOEXTI_TIMx, GPIOEXTI_GPIO, GPIOEXTI_Pin);
CrsfChannel      crsf_1;
CrsfChannel      crsf_2;
#if INPUT_PIN == LL_GPIO_PIN_2
Cereal_USART main_cer(2);
#elif INPUT_PIN == LL_GPIO_PIN_4
Cereal_USART main_cer(1);
#ifdef ENABLE_COMPILE_CLI
Cereal_TimerBitbang cli_cer(0);
#endif
#else
Cereal_USART main_cer(1);
#ifdef ENABLE_COMPILE_CLI
Cereal_TimerBitbang cli_cer(0);
#endif
#endif
#else
#error unsupported
#endif

#ifdef ENABLE_COMPILE_CLI
void boot_decide_cli(void);
void cli_enter(void);
#endif

int main(void)
{
    mcu_init();

    ENSURE_VERSION_DATA_IS_KEPT();

    #if defined(STM32F051DISCO)
    dbg_cer.begin(CLI_BAUD, false, true, false);
    #endif

    led_init();
    inp_init();
    pwm_init();
    pwm_all_flt();
    sense_init();

    eeprom_load_or_default();
    current_pid.Kp = cfg.currlim_kp;
    current_pid.Ki = cfg.currlim_ki;
    current_pid.Kd = cfg.currlim_kd;

    #ifdef ENABLE_COMPILE_CLI
    boot_decide_cli();
    #endif

    ledblink_disarmed();

    // setup inputs
    if (cfg.input_mode == INPUTMODE_RC)
    {
        rc_pulse_1.init();
        rc_pulse_2.init();
        rc1 = &rc_pulse_1;
        rc2 = &rc_pulse_2;
    }
    else if (cfg.input_mode == INPUTMODE_CRSF)
    {
        main_cer.init(cfg.baud == 0? CRSF_BAUDRATE : cfg.baud, false, true, false);
        crsf_1.init(&main_cer, cfg.channel_1);
        crsf_2.init(&main_cer, cfg.channel_2);
        rc1 = &crsf_1;
        rc2 = &crsf_2;
    }
    else
    {
        #ifdef ENABLE_COMPILE_CLI
        ledblink_cli();
        cli_enter(); // this never returns
        #else
        while (true)
        {
            // do nothing forever due to unconfigured input
            led_task();
        }
        #endif
    }

    braking = cfg.braking;
    complementary_pwm = true;
    arm_pulses_required = cfg.arm_duration;
    disarm_timeout = cfg.disarm_timeout;

    while (true) // main forever loop
    {
        led_task();
        sense_task();
        current_limit_task();
        rc1->task();
        rc2->task();

        bool armed1, armed2;
        int v1, v2;

        if (cfg.tied == false)
        {
            v1 = (armed1 = rc1->is_armed()) != false ? rc1->read() : 0;
            v2 = (armed2 = rc2->is_armed()) != false ? rc2->read() : 0;
        }
        else // controls are tied, take from one channel and apply to both
        {
            if ((armed1 = rc1->is_armed()) != false) {
                v2 = v1 = rc1->read();
            }
            else if ((armed2 = rc2->is_armed()) != false) {
                v2 = v1 = rc2->read();
            }
            else {
                v1 = v2 = 0;
            }
        }
        if (cfg.chan_swap) {
            int v3 = v1; v1 = v2; v2 = v3;
        }
        v1 = cfg.flip_1 ? -v1 : v1;
        v2 = cfg.flip_2 ? -v2 : v2;

        if (armed1 == false && armed2 == false) {
            ledblink_disarmed();
            pwm_all_flt();
            continue; // do not execute the rest of the logic
        }
        else if (v1 == 0 && v2 == 0) {
            ledblink_armed_stopped();
        }
        else {
            ledblink_moving();
        }

        int32_t duty_max;

        // impose temperature limiting if desired
        if (cfg.temperature_limit > 0 && sense_temperatureC > cfg.temperature_limit) {
            duty_max = fi_map(sense_temperatureC, cfg.temperature_limit, cfg.temperature_limit + TEMPERATURE_OVER, cfg.pwm_reload / 2, 1, true);
        }
        else {
            duty_max = cfg.pwm_reload - cfg.pwm_headroom;
        }

        // impose current limiting if desired
        if (cfg.current_limit > 0) {
            const int32_t duty_min = DEAD_TIME;
            if (current_limit_val < duty_min) {
                current_limit_val = duty_min;
            }
            if (duty_max > current_limit_val) {
                duty_max = current_limit_val;
            }
        }

        // impose voltage limit if desired
        if (cfg.voltage_limit > 0) {
            if (sense_voltage < cfg.voltage_limit) {
                duty_max -= fi_map(sense_voltage, cfg.voltage_limit, cfg.voltage_limit - UNDERVOLTAGE, 0, duty_max, true);
            }
        }

        uint32_t duty_mid = duty_max / 2; // this represents the center tap voltage if motors are going in opposite directions

        // determine the voltage boosting mode, either using a stick/switch value or using the mode set in EEPROM
        uint8_t boost_mode = ((cfg.channel_mode > 0) ? 
                fi_map(crsf_readChan(cfg.channel_mode), CRSF_CHANNEL_VALUE_1000, CRSF_CHANNEL_VALUE_2000, 0, 29, true) / 10
                :
                cfg.voltage_split_mode
                ); // % VSPLITMODE_END;

        int a1 = v1 >= 0 ? v1 : -v1;
        int a2 = v2 >= 0 ? v2 : -v2;
        int p1 = fi_map(a1, 0, THROTTLE_UNIT_RANGE, 0, duty_max, true);
        int p2 = fi_map(a2, 0, THROTTLE_UNIT_RANGE, 0, duty_max, true);
        int total_power = p1 + p2;
        bool use_half_voltage = false;

        if (v1 == 0 && v2 == 0) // both stopped, at least one is armed, so braking will be active if needed
        {
            pwm_set_all_duty_remapped(0, 0, 0);
        }
        else if (boost_mode == VSPLITMODE_BOOST_ALWAYS)
        {
            if ((v1 > 0 && v2 >= 0) || (v1 >= 0 && v2 > 0)) // both forward
            {
                pwm_set_all_duty_remapped(0,
                    fi_map(v1, 0, THROTTLE_UNIT_RANGE, 0, duty_max, true),
                    fi_map(v2, 0, THROTTLE_UNIT_RANGE, 0, duty_max, true));
            }
            else if ((v1 < 0 && v2 <= 0) || (v1 <= 0 && v2 < 0)) // both reverse
            {
                pwm_set_all_duty_remapped(duty_max,
                    duty_max - fi_map(-v1, 0, THROTTLE_UNIT_RANGE, 0, duty_max, true),
                    duty_max - fi_map(-v2, 0, THROTTLE_UNIT_RANGE, 0, duty_max, true));
            }
            else // directions oppose
            {
                int tries = 2;
                while (total_power > duty_max && tries--) // not enough voltage available, slow down proportionally
                {
                    p1 = fi_map(p1, 0, total_power, 0, duty_max, true);
                    p2 = fi_map(p2, 0, total_power, 0, duty_max, true);
                    total_power = p1 + p2;
                }
                if (v1 > v2) {
                    pwm_set_all_duty_remapped(p2, total_power, 0);
                }
                else {
                    pwm_set_all_duty_remapped(p1, 0, total_power);
                }
            }
        }
        else if (boost_mode == VSPLITMODE_BOOST_FORWARD)
        {
            int px = fi_map(p1, 0, duty_max, 0, duty_mid, true);
            int py = fi_map(p2, 0, duty_max, 0, duty_mid, true);
            int pm = px < py ? px : py;

            if (v1 >= 0 && v2 >= 0) // both forward
            {
                pwm_set_all_duty_remapped(duty_mid - pm, duty_mid + px, duty_mid + py);
            }
            else if (v1 <= 0 && v2 <= 0) // both reverse
            {
                pwm_set_all_duty_remapped(duty_mid + pm, duty_mid - px, duty_mid - py);
            }
            else // directions oppose
            {
                use_half_voltage = true;
            }
        }
        else // half-voltage or unknown
        {
            use_half_voltage = true;
        }

        if (use_half_voltage) {
            pwm_set_all_duty_remapped(duty_mid,
                fi_map(v1, -THROTTLE_UNIT_RANGE, THROTTLE_UNIT_RANGE, 0, duty_max, true),
                fi_map(v2, -THROTTLE_UNIT_RANGE, THROTTLE_UNIT_RANGE, 0, duty_max, true));
        }
    }

    return 0;
}

#ifdef ENABLE_COMPILE_CLI
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
#endif
