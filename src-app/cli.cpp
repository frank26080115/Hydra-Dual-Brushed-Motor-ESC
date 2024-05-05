#include "main.h"

// usage of the command-line-interface CLI is documented at https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/blob/master/doc/configuration.md

#ifdef ENABLE_COMPILE_CLI

#include <stdlib.h>
#include <stdio.h>

#include "cereal.h"
#include "swd_pins.h"

#include "inputpin.h"
#include "led.h"
#include "phaseout.h"
#include "userconfig.h"
#include "sense.h"
#include "rc.h"
#include "tone.h"
#include "version.h"

extern uint16_t current_limit_duty;
extern void current_limit_task(int v1, int v2);

#if defined(DEVELOPMENT_BOARD)
extern Cereal_USART dbg_cer;
extern RcChannel* rc1;
extern RcChannel* rc2;
#endif

extern Cereal_USART        main_cer;
extern Cereal_TimerBitbang cli_cer;

void cli_reportSensors(Cereal*);
void cli_execute(Cereal* cer, char* str);
int cmd_parseArgs(char* str);
void eeprom_print_all(Cereal* cer);

extern "C" {
extern const EEPROM_item_t cfg_items[];
extern const EEPROM_data_t cfge;
extern int eeprom_quick_validate(void);
#ifndef RELEASE_BUILD
extern uint8_t eeprom_error_log;
#endif
}

void cli_enter(void)
{
    tone_start(4, 0, cfg.tone_volume);
    tone_setVolume(0);
    ledblink_cli();
    swdpins_deinit();
    dbg_printf("CLI entered at %u\r\n", millis());

    #if defined(MAIN_SIGNAL_PA2)
        #if defined(ARTERY)
            // Artery chips have no echo problem on the USART peripheral
            Cereal_USART* cer = &main_cer;
            cer->init(CEREAL_ID_USART2, CLI_BAUD, true, false);
        #else
            // STM32 chips have an echo problem when using half-duplex mode
            // https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/1
            // I have attempted to fix the problem but it is still not 100% reliable
            // so I've simply defaulted to using bit-bang cereal instead
            //Cereal_TimerBitbang* cer = &cli_cer;
            //cer->init(CLI_BAUD);

            // update: Feb 5 2024
            // hmmm it feels like it can work with USART
            Cereal_USART* cer = &main_cer;
            cer->init(CEREAL_ID_USART2, CLI_BAUD, true, false);
        #endif
    #elif defined(MAIN_SIGNAL_PB4)
    Cereal_TimerBitbang* cer = &cli_cer;
    cer->init(CLI_BAUD);
    #else
    Cereal_TimerBitbang* cer = &cli_cer;
    cer->init(CLI_BAUD);
    #endif

    char buff[CLI_BUFF_SIZE];
    uint16_t buff_idx = 0;
    char prev = '\0';
    bool has_interaction = false;
    bool prev_has_interaction = false;
    uint32_t last_idle_prompt = 0;

    while (true)
    {
        uint32_t tnow = millis();
        led_task(false);

        int16_t c = cer->read();
        if (c >= 0) // data available
        {
            eeprom_delay_dirty();

            has_interaction |= ((c >= 'a' || c >= 'A') && c != '>');

            if (prev_has_interaction == false && has_interaction) {
                // stop tone on first good keystroke
                prev_has_interaction = true;
                tone_stop();
            }

            if (c == '\n' && prev == '\r')
            {
                // ignore windows style new line sequence
                prev = c;
                buff_idx = 0;
                continue;
            }
            else if (c == '\r' || c == '\n' || c == '\0')
            {
                // commit end line
                prev = c;
                #if CLI_ECHO
                cer->write('\r');
                cer->write('\n');
                #endif
                buff[buff_idx] = '\0';
                if (buff_idx > 0) {
                    cli_execute(cer, buff);
                }
                buff_idx = 0;
                cer->write('\r');
                cer->write('\n');
                cer->write('>'); // show prompt
                continue;
            }
            else if (c == 0x08 || c == 0x1B || c == 0x18 || c == 0x7F)
            {
                // these keys will clear the buffer, cancelling anything that's been written
                prev = c;
                #if CLI_ECHO
                cer->write('\r');
                cer->write('\n');
                #endif
                buff_idx = 0;
                cer->write('\r');
                cer->write('\n');
                cer->write('>');
                continue;
            }
            else if ((c == ' ' || c == '\t' || c == '>') && buff_idx <= 0)
            {
                // prevent leading spaces
                prev = c;
                continue;
            }
            else if ((c == ' ' || c == '\t') && (prev == ' ' || prev == '\t'))
            {
                // prevent double spaces
                prev = c;
                continue;
            }
            else
            {
                prev = c;
                if (buff_idx < CLI_BUFF_SIZE - 2) {
                    // place in buffer if there is room
                    c = c == '\t' ? ' ' : c; // convert tabs to spaces
                    buff[buff_idx] = c;
                    buff_idx++;
                    //buff[buff_idx] = 0;
                    #if CLI_ECHO
                    cer->write(c);
                    #endif
                }
                continue;
            }
        }
        else if (has_interaction == false && (tnow - last_idle_prompt) >= 3000) {
            last_idle_prompt = tnow;
            // if the user has not interacted at all with the CLI
            // then keep printing the prompt symbol periodically
            // just to aid the user
            cer->write('>');
        }

        // do other things
        #if defined(RELEASE_BUILD)
        eeprom_save_if_needed();
        #else
        if (eeprom_save_if_needed()) {
            int i = eeprom_quick_validate();
            if (i != 0) {
                cer->printf("ERROR: EEPROM save validation failed %d\r\n", i);
            }
            else {
                cer->printf("\r\nEEPROM saved!\r\n");
            }
        }
        #endif
        sense_task();

        #if defined(DISABLE_LED) || defined(ENABLE_TONE)
        if (has_interaction == false)
        {
            if ((millis() % 700) < 200) {
                tone_setVolume(cfg.tone_volume);
            }
            else {
                tone_setVolume(0);
            }
        }
        #endif
    }
}

void cli_execute(Cereal* cer, char* str)
{
    //int argc;
    if (item_strcmp("read", str, NULL))
    {
        cer->printf("all settings:\r\n\r\n");
        eeprom_print_all(cer);
        cer->write('\r');
        cer->write('\n');
        #ifndef RELEASE_BUILD
        cer->printf("write count: %lu\r\n", cfg.write_cnt);
        #endif
    }
    else if (item_strcmp("version", str, NULL))
    {
        cer->printf("\r\nV%u.%u E %u HW 0x%08lX N:%s\r\n", firmware_info.version_major, firmware_info.version_minor, firmware_info.version_eeprom, firmware_info.device_code, firmware_info.device_name);
        #ifndef RELEASE_BUILD
        cer->printf("input pin is GPIO P%c%u\r\n"
            , (char)(((uint32_t)'A') + ((firmware_info.device_code & 0xFF) / ((((uint32_t)GPIOB_BASE) - ((uint32_t)GPIOA_BASE)) >> 8)))
            , (uint8_t)((firmware_info.device_code >> 8) & 0xFF)
        );
        cer->printf("CPU freq %lu\r\n", (uint32_t)SystemCoreClock);
        #endif
    }
    else if (item_strcmp("factoryreset", str, NULL))
    {
        eeprom_factory_reset();
        cer->printf("\r\nfactory reset done");
    }
    else if (item_strcmp("reboot", str, NULL))
    {
        cer->printf("\r\nrebooting...\r\n");
        cer->flush();
        NVIC_SystemReset();
    }
    else if (item_strcmp("hwtest", str, NULL))
    {
        pwm_set_remap(cfg.phase_map);
        pwm_set_period(cfg.pwm_period);
        pwm_set_braking(true);
        pwm_set_all_duty_remapped(0, 0, 0);

        uint8_t  test_mode  = 0;
        uint16_t ori_vdiv   = cfg.voltage_divider;
        uint16_t ori_offset = cfg.current_offset;
        uint16_t ori_scale  = cfg.current_scale;
        uint16_t ori_dt     = cfg.pwm_deadtime;
        int8_t   phase_map  = cfg.phase_map;
        uint32_t tick = 0;
        uint32_t tnow;
        uint16_t pwr = 0, pwr100 = 0;
        int16_t c;
        while (true)
        {
            led_task(false);
            sense_task();
            tnow = millis();
            c = cer->read();

            // end the test
            if (c == 'x' || c == 0x08 || c == 0x1B || c == 0x18 || c == 0x7F) {
                break;
            }

            // trigger a new message on any key-stroke
            if (c > 0) {
                tick = 0;
            }

            if (test_mode == 'c')
            {
                switch (c)
                {
                    case ',' : cfg.current_offset   -=   1; break;
                    case '.' : cfg.current_offset   +=   1; break;
                    case '<' : cfg.current_offset   -=  10; break;
                    case '>' : cfg.current_offset   +=  10; break;
                    case ';' : cfg.current_scale    -=   1; break;
                    case '\'': cfg.current_scale    +=   1; break;
                    case ':' : cfg.current_scale    -=  10; break;
                    case '"' : cfg.current_scale    +=  10; break;
                }
            }
            else if (test_mode == 'v')
            {
                switch (c)
                {
                    case '[' : cfg.voltage_divider  -=   1; break;
                    case ']' : cfg.voltage_divider  +=   1; break;
                    case '{' : cfg.voltage_divider  -=  10; break;
                    case '}' : cfg.voltage_divider  +=  10; break;
                }
            }
            else if (test_mode == 'd' || test_mode == 'D')
            {
                switch (c)
                {
                    case '[' : cfg.pwm_deadtime  -=   1; break;
                    case ']' : cfg.pwm_deadtime  +=   1; break;
                    case '{' : cfg.pwm_deadtime  -=  10; break;
                    case '}' : cfg.pwm_deadtime  +=  10; break;
                }
            }

            if ((test_mode == 'c' || test_mode == 'p') && c >= '0' && c <= '9') {
                pwr    = fi_map(c, '0', '9', 0, PWM_DEFAULT_PERIOD, true);
                pwr100 = fi_map(c, '0', '9', 0, 100               , true);
                pwm_set_all_duty_remapped(pwr, 0, 0);
            }
            if (test_mode == 'p' && (c == '-' || c == '=' || c == '+')) {
                pwm_set_all_duty_remapped(0, 0, 0);
                phase_map = (c == '-') ? (phase_map - 1) : (phase_map + 1);
                phase_map = (phase_map <= 0) ? 3 : phase_map;
                phase_map = (phase_map >  3) ? 1 : phase_map;
                pwm_set_remap(phase_map);
                pwm_set_all_duty_remapped(pwr, 0, 0);
            }
            if (test_mode == 'd' || test_mode == 'D') {
                pwr = cfg.pwm_period - cfg.pwm_deadtime;
                pwr100 = 100;
                if (test_mode == 'd') {
                    pwm_set_all_duty_remapped(pwr, 0, 0);
                }
                else {
                    pwm_set_all_duty_remapped(0, pwr, pwr);
                }
            }

            if (c == 'v') {
                test_mode = c;
                pwr = 0;
                pwr100 = 0;
                pwm_set_all_duty_remapped(0, 0, 0);
            }
            else if (c == 'c' || c == 'p') {
                test_mode = c;
            }
            else if (c == 'd') {
                test_mode = (test_mode == 'd') ? 'D' : 'd';
            }

            if ((tnow - tick) >= 500) {
                if (test_mode == 'v') {
                    cer->printf("[%lu] raw-v %u , calc-v %lu , v-div %lu\r\n", tnow, adc_raw_voltage, sense_voltage, cfg.voltage_divider);
                }
                else if (test_mode == 'c') {
                    cer->printf("[%lu] raw-c %u , calc-c %lu , ", tnow, adc_raw_current, sense_current); cer->flush();
                    cer->printf("c-offset %lu , c-scale %lu , pwr %u\r\n", cfg.current_offset, cfg.current_scale, pwr100);
                }
                else if (test_mode == 'p') {
                    cer->printf("[%lu] phase-map %u , pwr %u\r\n", tnow, phase_map, pwr100);
                }
                else if (test_mode == 'd' || test_mode == 'D') {
                    cer->printf("[%lu] pwr %u ; DT %lu ; (press 'v' to stop)\r\n", tnow, pwr100, cfg.pwm_deadtime);
                }
                else {
                    cer->printf("[%lu] press 'v' or 'c' or 'p' or 'd'\r\n", tnow);
                }
                tick = tnow;
            }
        }
        pwm_set_all_duty_remapped(0, 0, 0);

        cer->printf("\r\nsession end, voltdiv %lu , ", cfg.voltage_divider); cer->flush();
        cer->printf("curroffset %lu , currscale %lu , pwm_deadtime %lu , ", cfg.current_offset, cfg.current_scale, cfg.pwm_deadtime); cer->flush();
        cer->printf("phasemap %d \r\n(not saved !!!)\r\n", phase_map);

        cfg.voltage_divider = ori_vdiv;
        cfg.current_offset  = ori_offset;
        cfg.current_scale   = ori_scale;
        cfg.pwm_deadtime    = ori_dt;
        pwm_set_remap(cfg.phase_map);
    }
    #ifndef RELEASE_BUILD
    else if (item_strcmp("viewlog", str, NULL))
    {
        cer->printf("\r\nEEPROM load log = %d\r\n", eeprom_error_log);
    }
    #endif
    else
    {
        // none of the commands match, loop through the potential settings to see if it's something to save
        int32_t nv;
        bool res = eeprom_user_edit(str, &nv);
        if (res) {
            cer->printf("\r\nOK (%s = %li)", str, nv);
            load_runtime_configs();
            eeprom_unlock_key = EEPROM_MAGIC;
            return;
        }
        else {
            // unrecognized command
            cer->printf("\r\nERROR CMD \"%s\"", str);
            return;
        }
    }
}

void eeprom_print_all(Cereal* cer)
{
    int i;
    for (i = 0; ; i++)
    {
        EEPROM_item_t* desc = (EEPROM_item_t*)&(cfg_items[i]);
        if (desc->ptr == 0) {
            return;
        }
        uint32_t ptrstart = (uint32_t)&cfge;
        uint32_t itmidx = desc->ptr - ptrstart;
        uint8_t* ptr8 = (uint8_t*)&cfg;
        int32_t v = 0;
        memcpy(&v, &(ptr8[itmidx]), desc->size);
        dbg_printf("%u %u \t ", itmidx, desc->size);
        cer->printf("%s %li\r\n", desc->name, v);
    }
}

extern uint8_t crsf_inputGuess;

#ifdef DEVELOPMENT_BOARD
void cliboot_if_key(void)
{
    if (dbg_cer.available() >= 3 && dbg_cer.peekTail() == '\n') {
        dbg_cer.reset_buffer();
        dbg_printf("CLI enter from keys\r\n");
        cli_enter();
    }
}
#else
#define cliboot_if_key(...)
#endif

void cliboot_decide(void)
{
    uint32_t tstart;
    inp_init();
    inp2_init();

    ledblink_cliwait();

    int i;
    for (i = 0; i < CLI_ENTER_UNPLUG_CRITERIA; i++)
    {
        inp_pullDown();
        inp2_pullDown();
        if (cfg.input_mode == INPUTMODE_RC_SWD || cfg.input_mode == INPUTMODE_CRSF_SWCLK) {
            swclk_init(LL_GPIO_PULL_DOWN);
        }

        tone_start(2, 150, cfg.tone_volume);

        tstart = millis();
        while ((millis() - tstart) < 3) {
            // wait for pull resistors to take effect
            if (inp_read() == 0
            #if defined(MAIN_SIGNAL_PB4)
            && ((cfg.input_mode == INPUTMODE_CRSF && inp2_read() == 0) || cfg.input_mode != INPUTMODE_CRSF)
            #endif
            && ((cfg.input_mode == INPUTMODE_CRSF_SWCLK && swclk_read() == 0) || cfg.input_mode != INPUTMODE_CRSF_SWCLK)
            )
            {
                break;
            }
        }

        bool is_1 = false, is_2 = false;
        while ((millis() - tstart) < 500)
        {
            led_task(false);
            cliboot_if_key();
            if ((is_2 |= (inp_read() != 0)) || (is_1 |= (inp2_read() != 0)) ||
                    ((cfg.input_mode == INPUTMODE_RC_SWD || cfg.input_mode == INPUTMODE_CRSF_SWCLK) && swclk_read() != 0))
            {
                dbg_printf("CLI cancel from signal high\r\n");
                if (is_1) {
                    crsf_inputGuess = 1;
                }
                else if (is_2) {
                    crsf_inputGuess = 2;
                }

                if (is_2)
                {
                    tstart = millis();
                    while (true)
                    {
                        led_task(false);
                        if (inp_read() == 0 || inp2_read() != 0 || ((cfg.input_mode == INPUTMODE_RC_SWD || cfg.input_mode == INPUTMODE_CRSF_SWCLK) && swclk_read() != 0)) {
                            // this will happen if a CRSF receiver is connected and transmissions start to happen
                            dbg_printf("reboot cancel from signal low\r\n");
                            return;
                        }
                        if ((millis() - tstart) >= 5000)
                        {
                            // if it reboots but exits the bootloader, it will probably end up at the `(inp_read() == 0)` check above
                            dbg_printf("reset into bootloader\r\n");
                            NVIC_SystemReset(); // to back to bootloader
                        }
                    }
                }

                return;
            }
        }
        tstart = millis();
        inp_pullUp();
        inp2_pullUp();
        if (cfg.input_mode == INPUTMODE_RC_SWD || cfg.input_mode == INPUTMODE_CRSF_SWCLK) {
            swclk_init(LL_GPIO_PULL_UP);
        }
        while ((millis() - tstart) < 3)
        {
            // wait for pull resistors to take effect
            if (inp_read() != 0
            #if defined(MAIN_SIGNAL_PB4)
            && ((cfg.input_mode == INPUTMODE_CRSF && inp2_read() != 0) || cfg.input_mode != INPUTMODE_CRSF)
            #endif
            && ((cfg.input_mode == INPUTMODE_CRSF_SWCLK && swclk_read() != 0) || cfg.input_mode != INPUTMODE_CRSF_SWCLK)
            )
            {
                break;
            }
        }
        while ((millis() - tstart) < 500)
        {
            led_task(false);
            cliboot_if_key();
            if (inp_read() == 0 || inp2_read() == 0 ||
                    ((cfg.input_mode == INPUTMODE_RC_SWD || cfg.input_mode == INPUTMODE_CRSF_SWCLK) && swclk_read() == 0))
            {
                dbg_printf("CLI cancel from signal low\r\n");
                return;
            }
        }
    }

    // pull down main signal input again and wait for it to settle
    inp_pullDown();
    tstart = millis();
    while ((millis() - tstart) < 3) {
        if (inp_read() == 0) {
            break;
        }
    }

    // wait for user to plug in
    ledblink_cliplug();
    tstart = millis();
    tone_start(2, 0, cfg.tone_volume);
    while (true)
    {
        led_task(false);

        #if defined(DISABLE_LED) || defined(ENABLE_TONE)
        uint32_t tspan = millis() - tstart;
        if ((tspan % 600) < 200) {
            tone_setVolume(cfg.tone_volume);
        }
        else {
            tone_setVolume(0);
        }
        #endif

        if (inp_read() != 0) {
            cli_enter();
        }
    }
}

#endif // ENABLE_COMPILE_CLI
