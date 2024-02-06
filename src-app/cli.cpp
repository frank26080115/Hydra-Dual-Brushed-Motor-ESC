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

bool cli_hwdebug;
extern int16_t current_limit_val;
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
            cer->init(CEREAL_ID_USART2, CLI_BAUD, true, false, false);
        #else
            // STM32 chips have an echo problem when using half-duplex mode
            // https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/1
            // I have attempted to fix the problem but it is still not 100% reliable
            // so I've simply defaulted to using bit-bang cereal instead
            Cereal_TimerBitbang* cer = &cli_cer;
            cer->init(CLI_BAUD);
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
            has_interaction |= c >= 'a' || c >= 'A';

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
                    if (buff[buff_idx - 1] == ' ') {
                        // trim away trailing space
                        buff[buff_idx - 1] = '\0';
                    }
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
            else if ((c == ' ' || c == '\t') && buff_idx <= 0)
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
                    buff[buff_idx] = 0;
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
        tnow = millis();
        eeprom_save_if_needed();
        if (cli_hwdebug) {
            // report analog values if desired
            sense_task();
            static uint32_t last_hwdebug = 0;
            if ((tnow - last_hwdebug) >= 200) {
                last_hwdebug = tnow;
                cli_reportSensors(cer);
            }
        }
    }
}

void cli_execute(Cereal* cer, char* str)
{
    int argc;
    if (item_strcmp("read", str))
    {
        cer->printf("all settings:\r\n\r\n");
        eeprom_print_all(cer);
        cer->write('\r');
        cer->write('\n');
    }
    else if (item_strcmp("version", str))
    {
        cer->printf("\r\nV%u.%u E %u HW 0x%08lX N:%s\r\n", firmware_info.version_major, firmware_info.version_minor, firmware_info.version_eeprom, firmware_info.device_code, firmware_info.device_name);
        cer->printf("input pin is GPIO P%c%u\r\n"
            , (char)(((uint32_t)'A') + ((firmware_info.device_code & 0xFF) / ((((uint32_t)GPIOB_BASE) - ((uint32_t)GPIOA_BASE)) >> 8)))
            , (uint8_t)((firmware_info.device_code >> 8) & 0xFF)
        );
        cer->printf("CPU frequency %lu\r\n", (uint32_t)SystemCoreClock);
    }
    else if (item_strcmp("hwdebug", str))
    {
        argc = cmd_parseArgs(str);
        if (argc <= 0) {
            // no arguments means toggle
            cli_hwdebug = !cli_hwdebug;
        }
        else {
            // 0 means false
            cli_hwdebug = cmd_args[0] != 0;
        }
        if (cli_hwdebug) {
            #ifdef STMICRO
            swdio_init(LL_GPIO_PULL_UP);
            swclk_init(LL_GPIO_PULL_UP);
            #endif
        }
        cer->printf("\r\nhardware debug: %u\r\n", cli_hwdebug);
        // cli_hwdebug will cause periodic printout of analog sensor values
    }
    else if (item_strcmp("factoryreset", str))
    {
        eeprom_factory_reset();
        cer->printf("\r\nfactory reset EEPROM done");
    }
    else if (item_strcmp("reboot", str))
    {
        cer->printf("\r\nrebooting...\r\n");
        cer->flush();
        NVIC_SystemReset();
    }
    else if (item_strcmp("testpwm", str))
    {
        // pulse a pin with a certain voltage
        // useful for identifying which pin is which
        // keep the pulse short to avoid damage
        // analog sensor values are printed during the test
        // this can be used to test current limits

        // args:
        // which_phase    power    duration

        argc = cmd_parseArgs(str);
        if (argc < 3 || cmd_args[0] <= 0) {
            cer->printf("\r\nERROR ARGS");
            return;
        }

        pwm_set_period(PWM_DEFAULT_PERIOD);
        pwm_set_braking(true);

        uint16_t pwr = fi_map(cmd_args[1], 0, 100, 0, PWM_DEFAULT_PERIOD, true);

        int phase = ((cmd_args[0] - 1) % 3) + 1;
        uint32_t duration = cmd_args[2];
        pwm_set_remap(phase);

        cer->printf("\r\ntesting PWM, phase %u, pwr = %u%%, time = %lums\r\n"
                , phase
                , pwr
                , duration
                );

        uint32_t max_current = 0;

        pwm_set_all_duty_remapped(pwr, 0, 0);

        cer->reset_buffer();
        uint32_t t = 0;
        do
        {
            uint32_t tnow = millis();
            led_task(false);
            // check sensors and perform current limit calculations
            if (sense_task()) {
                current_limit_task();
                cli_reportSensors(cer); // print if new data available
                if (t == 0) {
                    t = tnow;
                }

                if (sense_current > max_current) {
                    max_current = sense_current;
                }
            }
            if (t != 0 && duration > 0 && (tnow - t) >= duration) {
                // quit if time expired
                break;
            }
            if (t != 0) {
                // quit on key press
                int16_t nc = cer->read();
                if (nc > 0) {
                    break;
                }
            }
        } while (true);

        pwm_set_all_duty_remapped(0, 0, 0); // end
        cer->printf("\r\ntest end, maximum detect current = %ld", max_current);
    }
    else if (item_strcmp("tunevoltage", str))
    {
        uint16_t original = cfg.voltage_divider;
        uint32_t tick = 0;
        uint32_t tnow;
        int16_t c;
        while (true)
        {
            sense_task();
            tnow = millis();
            int16_t c = cer->read();
            if (c == '\n' || c == '\r' || c == '\0' || c == 'x' || c == 's' || c == 0x08 || c == 0x1B || c == 0x18 || c == 0x7F) {
                break;
            }
            switch (c)
            {
                case ',' : cfg.voltage_divider -=    1; break;
                case '.' : cfg.voltage_divider +=    1; break;
                case ';' : cfg.voltage_divider -=   10; break;
                case '\'': cfg.voltage_divider +=   10; break;
            }
            if ((tnow - tick) >= 200) {
                cer->printf("[%lu] raw %u , calc %lu , vdiv %lu\r\n", tnow, adc_raw_voltage, sense_voltage, cfg.voltage_divider);
                tick = tnow;
            }
        }
        cer->printf("\r\ntunevoltage session end, vdiv = %lu", cfg.voltage_divider);
        if (c == 's') { // save
            cer->printf(" , saving\r\n");
            eeprom_save();
        }
        else {
            cer->printf(" , not saved\r\n");
            cfg.voltage_divider = original;
        }
    }
    else if (item_strcmp("tunecurrent", str))
    {
        pwm_set_period(PWM_DEFAULT_PERIOD);
        pwm_set_braking(true);
        pwm_set_all_duty_remapped(0, 0, 0);

        uint16_t ori_offset = cfg.current_offset;
        uint16_t ori_scale = cfg.current_scale;
        uint32_t tick = 0;
        uint32_t tnow;
        uint16_t pwr, pwr100;
        int16_t c;
        while (true)
        {
            sense_task();
            tnow = millis();
            int16_t c = cer->read();
            if (c == '\n' || c == '\r' || c == '\0' || c == 'x' || c == 's' || c == 0x08 || c == 0x1B || c == 0x18 || c == 0x7F) {
                break;
            }
            switch (c)
            {
                case ',' : cfg.current_offset -=   1; break;
                case '.' : cfg.current_offset +=   1; break;
                case '<' : cfg.current_offset -=  10; break;
                case '>' : cfg.current_offset +=  10; break;
                case ';' : cfg.current_scale  -=   1; break;
                case '\'': cfg.current_scale  +=   1; break;
                case ':' : cfg.current_scale  -=  10; break;
                case '"' : cfg.current_scale  +=  10; break;
            }
            if (c >= '0' && c <= '9') {
                pwr    = fi_map(c, '0', '9', 0, PWM_DEFAULT_PERIOD, true);
                pwr100 = fi_map(c, '0', '9', 0, 100               , true);
                pwm_set_all_duty_remapped(pwr, 0, 0);
            }
            if ((tnow - tick) >= 200) {
                cer->printf("[%lu] raw %u , calc %lu , offset %lu , scale %lu , pwr %u\r\n", tnow, adc_raw_current, sense_current, cfg.current_offset, cfg.current_scale, pwr100);
                tick = tnow;
            }
        }
        pwm_set_all_duty_remapped(0, 0, 0);
        cer->printf("\r\ntunecurrent session end, offset = %lu, scale = %lu", cfg.current_offset, cfg.current_scale);
        if (c == 's') { // save
            cer->printf(" , saving\r\n");
            eeprom_save();
        }
        else {
            cer->printf(" , not saved\r\n");
            cfg.current_offset = ori_offset;
            cfg.current_scale  = ori_scale;
        }
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

int cmd_parseArgs(char* str) // parse a list of integers delimited by space
{
    int i = 0;
    char * token = strtok(str, " ");
    while( token != NULL && i < MAX_CMD_ARGS)
    {
        if (i != 0)
        {
            int32_t x = parse_integer((const char*)token);
            cmd_args[i-1] = x;
        }
        token = strtok(NULL, " ");
        i += 1;
   }
   return i;
}

void cli_reportSensors(Cereal* cer)
{
    cer->printf("[%lu]: ", millis());
    #if defined(DEVELOPMENT_BOARD)
    if (rc1 != NULL && rc2 != NULL)
    {
        cer->printf("RC1=");
        if (rc1->is_alive()) {
            cer->printf("%d, ", rc1->read());
        }
        else {
            cer->printf("?, ");
        }
        cer->printf("RC2=");
        if (rc2->is_alive()) {
            cer->printf("%d, ", rc2->read());
        }
        else {
            cer->printf("?, ");
        }
    }
    #endif
    // for ESCs without a telemetry pad, enable the reading of the SWD pins
    // these will be pulled-up, and the user can short them to ground to see which pad responds
    cer->printf("SWDIO=%d, SWCLK=%d, ", swdio_read() ? 1 : 0, swclk_read() ? 1 : 0);
    cer->printf("T=%ld, V=%ld, C=%ld, Currlim=%d, \r\n", sense_temperatureC, sense_voltage, sense_current, current_limit_val);
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
