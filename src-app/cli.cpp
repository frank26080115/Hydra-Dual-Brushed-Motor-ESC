#include "main.h"

#ifdef ENABLE_COMPILE_CLI
#ifndef ENABLE_CEREAL_TX
#error
#endif

#include <stdlib.h>
#include <stdio.h>

#include "cereal.h"

#ifdef STMICRO
#include "rc_stm32.h"
#include "cereal_timer.h"
#include "cereal_usart.h"
#include "swd_pins.h"
#endif

#include "led.h"
#include "phaseout.h"
#include "userconfig.h"
#include "sense.h"
#include "rc.h"

#define MAX_CMD_ARGS 8
int32_t cmd_args[MAX_CMD_ARGS];

uint8_t cli_phaseremap;
bool cli_hwdebug;
extern int16_t current_limit_val;
extern void current_limit_task(void);

#if defined(DEVELOPMENT_BOARD)
extern Cereal_USART dbg_cer;
extern RcChannel* rc1;
extern RcChannel* rc2;
#endif

#ifdef STMICRO
extern Cereal_USART main_cer;

#if INPUT_PIN == LL_GPIO_PIN_2
//
#elif INPUT_PIN == LL_GPIO_PIN_4

#ifdef ENABLE_COMPILE_CLI
extern Cereal_TimerBitbang cli_cer;
#endif
#else

#ifdef ENABLE_COMPILE_CLI
extern Cereal_TimerBitbang cli_cer;
#endif
#endif
#else
#error unsupported
#endif

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
    ledblink_cli();
    dbg_printf("CLI entered at %u\r\n", millis());

    #if defined(DEVELOPMENT_BOARD)
    Cereal_USART* cer = &dbg_cer;
    // init already called on dbg_cer
    // since we are using the debugging UART, the two pins can be used for testing RC inputs
    rc1->init();
    rc2->init();
    #elif INPUT_PIN == LL_GPIO_PIN_2
    Cereal_USART* cer = &main_cer;
    cer->init(CEREAL_ID_USART2, CLI_BAUD, false, true, false);
    #elif INPUT_PIN == LL_GPIO_PIN_4
    Cereal_TimerBitbang* cer = &cli_cer;
    cer->init(CLI_BAUD);
    #else
    Cereal_TimerBitbang* cer = &cli_cer;
    cer->init(CLI_BAUD);
    #endif

    cli_phaseremap = cfg.phase_map;

    char buff[CLI_BUFF_SIZE];
    uint16_t buff_idx = 0;
    char prev = '\0';

    while (true)
    {
        led_task(false);

        int16_t c = cer->read();
        if (c >= 0) // data available
        {
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
            else if (c == 0x08 || c == 0x1B || c == 0x7F)
            {
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

        // do other things
        eeprom_save_if_needed();
        if (cli_hwdebug) {
            // report analog values if desired
            sense_task();
            static uint32_t last_hwdebug = 0;
            if ((millis() - last_hwdebug) >= 200) {
                last_hwdebug = millis();
                cli_reportSensors(cer);
            }
        }
    }
}

void cli_execute(Cereal* cer, char* str)
{
    int argc;
    if (item_strcmp("list", str))
    {
        cer->printf("all settings:\r\n");
        eeprom_print_all(cer);
        cer->write('\r');
        cer->write('\n');
    }
    else if (item_strcmp("whichinput", str))
    {
        cer->printf("\r\nInput Pin: P");
        if (INPUT_PIN_PORT == GPIOA) {
            cer->printf("A");
        }
        else if (INPUT_PIN_PORT == GPIOB) {
            cer->printf("B");
        }
        else {
            cer->printf("?");
        }
        for (int i = 0; i < 32; i++) {
            if (INPUT_PIN == (1 << i)) {
                cer->printf("%u\r\n", i);
            }
        }
    }
    else if (item_strcmp("hwdebug", str))
    {
        argc = cmd_parseArgs(str);
        if (argc <= 0) {
            // no arguments means toggle
            cli_hwdebug ^= true;
        }
        else {
            // 0 means false
            cli_hwdebug = cmd_args[0] != 0;
        }
        if (cli_hwdebug) {
            #ifdef STMICRO
            swdpins_init(LL_GPIO_PULL_UP);
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
    else if (item_strcmp("testpwm", str))
    {
        // pulse a pin with a certain voltage
        // useful for identifying which pin is which
        // keep the pulse short to avoid damage
        // analog sensor values are printed during the test
        // this can be used to test current limits

        argc = cmd_parseArgs(str);
        if (argc < 3 || cmd_args[0] <= 0) {
            cer->printf("\r\nERROR");
            return;
        }

        pwm_set_remap(cli_phaseremap); // set by command "testremap"

        int phase = (cmd_args[0] - 1) % 3;
        switch (phase)
        {
            case 0: pwm_set_all_duty_remapped(cmd_args[1], 0, 0); break;
            case 1: pwm_set_all_duty_remapped(0, cmd_args[1], 0); break;
            case 2: pwm_set_all_duty_remapped(0, 0, cmd_args[1]); break;
        }

        cer->reset_buffer();
        uint32_t t = 0;
        do
        {
            led_task(false);
            // check sensors and perform current limit calculations
            if (sense_task()) {
                current_limit_task();
                cli_reportSensors(cer); // print if new data available
                if (t == 0) {
                    t = millis();
                }
            }
            if (t != 0 && cmd_args[2] > 0 && (int)(millis() - t) >= cmd_args[2]) {
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
        cer->printf("\r\ntest end");
    }
    else if (item_strcmp("testremap", str))
    {
        // changes the phase remapping temporarily for testing purposes
        // it is not committed to EEPROM
        argc = cmd_parseArgs(str);
        if (argc < 1) {
            cer->printf("\r\nERROR");
            return;
        }
        cli_phaseremap = cmd_args[0];
        cer->printf("\r\ntest remap val = %u", cli_phaseremap);
    }
    else
    {
        // none of the commands match, loop through the potential settings to see if it's something to save
        bool res = eeprom_user_edit(str, NULL);
        if (res) {
            cer->printf("\r\nOK");
            return;
        }
        else {
            cer->printf("\r\nERROR");
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
   return 0;
}

void cli_reportSensors(Cereal* cer)
{
    cer->printf("[%lu]: ", millis());
    #if defined(STM32F051DISCO)
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
    #endif
    #ifdef STMICRO
    // for ESCs without a telemetry pad, enable the reading of the SWD pins
    // these will be pulled-up, and the user can short them to ground to see which pad responds
    cer->printf("SWDIO=%d, SWCLK=%d, ", swdio_read() ? 1 : 0, swclk_read() ? 1 : 0);
    #endif
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
        itmidx += (uint32_t)&cfg;
        uint8_t* ptr8 = (uint8_t*)&cfg;
        int32_t v = 0;
        memcpy(&v, &(ptr8[itmidx]), desc->size);
        cer->printf("%s %li\r\n", desc->name, v);
    }
}

#endif // ENABLE_COMPILE_CLI