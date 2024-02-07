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

#define MAX_CMD_ARGS 8
int32_t cmd_args[MAX_CMD_ARGS];

extern uint16_t current_limit_duty;
extern void current_limit_task(void);

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
}

void cli_enter(void)
{
    tone_start(4, 2000, cfg.tone_volume);
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
        eeprom_save_if_needed();
        sense_task();
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
    }
    else if (item_strcmp("version", str, NULL))
    {
        cer->printf("\r\nV%u.%u E %u HW 0x%08lX N:%s\r\n", firmware_info.version_major, firmware_info.version_minor, firmware_info.version_eeprom, firmware_info.device_code, firmware_info.device_name);
        cer->printf("input pin is GPIO P%c%u\r\n"
            , (char)(((uint32_t)'A') + ((firmware_info.device_code & 0xFF) / ((((uint32_t)GPIOB_BASE) - ((uint32_t)GPIOA_BASE)) >> 8)))
            , (uint8_t)((firmware_info.device_code >> 8) & 0xFF)
        );
        //cer->printf("CPU freq %lu\r\n", (uint32_t)SystemCoreClock);
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
        pwm_set_period(PWM_DEFAULT_PERIOD);
        pwm_set_braking(true);
        pwm_set_all_duty_remapped(0, 0, 0);

        uint8_t  test_mode  = 0;
        uint16_t ori_vdiv   = cfg.voltage_divider;
        uint16_t ori_offset = cfg.current_offset;
        uint16_t ori_scale  = cfg.current_scale;
        int8_t   phase_map  = cfg.phase_map;
        uint32_t tick = 0;
        uint32_t tnow;
        uint16_t pwr = 0, pwr100 = 0;
        int16_t c;
        while (true)
        {
            sense_task();
            tnow = millis();
            c = cer->read();
            if (c == 'x' || c == 0x08 || c == 0x1B || c == 0x18 || c == 0x7F) {
                break;
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

            if ((test_mode == 'c' || test_mode == 'p') && c >= '0' && c <= '9') {
                pwr    = fi_map(c, '0', '9', 0, PWM_DEFAULT_PERIOD, true);
                pwr100 = fi_map(c, '0', '9', 0, 100               , true);
                pwm_set_all_duty_remapped(pwr, 0, 0);
            }
            if (test_mode == 'p' && (c == '-' || c == '=' || c == '+')) {
                pwm_set_all_duty_remapped(0, 0, 0);
                phase_map = (c == '-') ? (phase_map - 1) : (phase_map + 1);
                phase_map = (phase_map <= 0) ? 2 : phase_map;
                phase_map = (phase_map >  3) ? 1 : phase_map;
                pwm_set_remap(phase_map);
                pwm_set_all_duty_remapped(pwr, 0, 0);
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

            if ((tnow - tick) >= 250) {
                if (test_mode == 'v') {
                    cer->printf("[%lu] raw-v %u , calc-v %lu , v-div %lu\r\n", tnow, adc_raw_voltage, sense_voltage, cfg.voltage_divider);
                }
                else if (test_mode == 'c') {
                    cer->printf("[%lu] raw-c %u , calc-c %lu , c-offset %lu , c-scale %lu , pwr %u\r\n", tnow, adc_raw_current, sense_current, cfg.current_offset, cfg.current_scale, pwr100);
                }
                else if (test_mode == 'p') {
                    cer->printf("[%lu] phase-map %u , pwr %u\r\n", tnow, phase_map, pwr100);
                }
                else {
                    cer->printf("[%lu] press 'v' or 'c' or 'p'\r\n", tnow);
                }
                tick = tnow;
            }
        }
        pwm_set_all_duty_remapped(0, 0, 0);

        cer->printf("\r\nsession end, voltdiv %lu , curroffset %lu , currscale %lu , phasemap %d \r\n(not saved !!!)\r\n", cfg.voltage_divider, cfg.current_offset, cfg.current_scale, phase_map);

        cfg.voltage_divider = ori_vdiv;
        cfg.current_offset  = ori_offset;
        cfg.current_scale   = ori_scale;
        pwm_set_remap(cfg.phase_map);
    }
    else
    {
        // none of the commands match, loop through the potential settings to see if it's something to save
        int32_t nv;
        bool res = eeprom_user_edit(str, &nv);
        if (res) {
            cer->printf("\r\nOK (%s = %li)", str, nv);
            load_runtime_configs();
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

bool cliboot_if_2nd_sig(void)
{
    bool inp2 = false;
    if ((inp2 |= inp2_read()) != 0
        #ifdef DEVELOPMENT_BOARD
            || ((cfg.input_mode == INPUTMODE_CRSF_SWCLK || cfg.input_mode == INPUTMODE_RC_SWD) && swclk_read() != 0)
        #endif
         ) {
        crsf_inputGuess = 1; // PB6/USART1
        dbg_printf("CLI cancel from 2ndary signal high\r\n");
        return true;
    }
    return false;
}

#define CLIBOOT_IF_2ND_SIG()    do { if (cliboot_if_2nd_sig()) { return; } } while (0)

void cliboot_decide(void)
{
    inp_init();
    inp2_init();
    inp_pullDown();
    inp2_pullDown();

    if (cfg.input_mode == INPUTMODE_RC_SWD || cfg.input_mode == INPUTMODE_CRSF_SWCLK)
    {
        swclk_init(LL_GPIO_PULL_DOWN);
    }

    ledblink_cliwait();

    uint32_t tstart = millis();
    while ((millis() - tstart) < 2) {
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
    tstart = millis();

    if (inp_read() == 0)
    {
        dbg_printf("CLI potential - input is low\r\n");

        while ((millis() - tstart) < 100)
        {
            led_task(false);
            cliboot_if_key();
            CLIBOOT_IF_2ND_SIG();

            if (inp_read() != 0)
            {
                crsf_inputGuess = 2;
                dbg_printf("CLI cancel from signal high\r\n");
                return;
            }
        }
        dbg_printf("CLI stage - 100ms passed\r\n");
        inp_pullUp();
        while ((millis() - tstart) < CLI_ENTER_LOW_CRITERIA)
        {
            led_task(false);
            cliboot_if_key();
            CLIBOOT_IF_2ND_SIG();

            if (inp_read() == 0)
            {
                crsf_inputGuess = 2;
                dbg_printf("CLI cancel from signal low\r\n"); // meaning a device is connected but driving it low, probably a PWM receiver that's not yet sending pulses
                // CLI requires the signal to be completely unplugged
                return;
            }
        }
        dbg_printf("CLI stage - long unplug criteria passed\r\n");
        ledblink_cliplug();
    }
    else
    {
        tstart = millis();
        while (true)
        {
            led_task(true);
            cliboot_if_key();
            CLIBOOT_IF_2ND_SIG();
            if (inp_read() == 0) {
                // this will happen if a CRSF receiver is connected and transmissions start to happen
                dbg_printf("reboot cancel from signal low\r\n");
                return;
            }
            if ((millis() - tstart) >= 3000)
            {
                // if it reboots but exits the bootloader, it will probably end up at the `(inp_read() == 0)` check above
                dbg_printf("reset into bootloader\r\n");
                NVIC_SystemReset(); // to back to bootloader
            }
        }
    }

    tone_start(2, 0, cfg.tone_volume);
    inp_init();
    inp_pullDown();
    tstart = millis();
    uint8_t pulse_cnt = 0;
    bool was_high = inp_read();
    uint32_t th = was_high ? tstart : 0;
    //uint32_t tl = was_high ? 0 : tstart;
    while (true)
    {
        led_task(false);
        cliboot_if_key();

        if (inp_read() != 0)
        {
            if (was_high == false) {
                th = millis();
            }
            if ((millis() - th) >= CLI_ENTER_HIGH_CRITERIA) {
                dbg_printf("CLI enter from insertion\r\n");
                cli_enter();
            }
            was_high = true;
        }
        else
        {
            if (was_high != false) {
                pulse_cnt++;
                if (pulse_cnt >= 20) {
                    dbg_printf("CLI cancel from signal pulses (too many pulses)\r\n");
                    tone_stop();
                    return;
                }
            }
            was_high = false;
        }

        CLIBOOT_IF_2ND_SIG();
    }
}

#endif // ENABLE_COMPILE_CLI
