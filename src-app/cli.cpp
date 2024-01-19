#include "main.h"
#include "cereal.h"

#ifdef STMICRO
#include "cereal_timer.h"
#include "cereal_usart.h"
#endif

#define MAX_CMD_ARGS 8
int32_t* cmd_args;

uint8_t cli_phaseremap;

Cereal* cli_cereal;

void cli_enter(void)
{
    #if INPUT_PIN == LL_GPIO_PIN_2
    Cereal_USART* cer = new Cereal_USART(2, CLI_BUFF_SIZE);
    cer->begin(CLI_BAUD, false, true, false);
    #elif INPUT_PIN == LL_GPIO_PIN_4
    Cereal_TimerBitbang* cer = new Cereal_TimerBitbang(0, CLI_BUFF_SIZE);
    cer->begin(CLI_BAUD);
    #endif

    cli_cereal = cer;

    cmd_args = (int32_t*)malloc(MAX_CMD_ARGS * sizeof(int32_t)); // do not waste RAM unless we are in CLI mode

    cli_phaseremap = cfg->phase_map;

    char buff[CLI_BUFF_SIZE];
    uint16_t buff_idx = 0;
    char prev = '\0';

    while (true)
    {
        led_task();

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
            sensor_task();
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
        cer->printf("all settings:\r\n")
        eeprom_print_all(cer);
        cer->write('\r');
        cer->write('\n');
    }
    else if (item_strcmp("hwdebug", str))
    {
        argc = cmd_parseArgs(str);
        if (argc <= 0) {
            cli_hwdebug ^= true;
        }
        else {
            cli_hwdebug = cmd_args[0] != 0;
        }
        cer->printf("hardware debug: %u\r\n", cli_hwdebug);
        // cli_hwdebug will cause periodic printout of analog sensor values
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

        int phase = (cmd_args[0] - 1) % 3;
        switch (phase)
        {
            case 0: pwm_set_all_duty_remapped(cmd_args[1], 0, 0, cli_phaseremap); break;
            case 1: pwm_set_all_duty_remapped(0, cmd_args[1], 0, cli_phaseremap); break;
            case 2: pwm_set_all_duty_remapped(0, 0, cmd_args[1], cli_phaseremap); break;
        }

        cer->reset_buffer();
        uint32_t t = 0;
        do
        {
            led_task();
            // check sensors and perform current limit calculations
            if (sensor_task()) {
                current_limit_task();
                cli_reportSensors(cer); // print if new data available
                if (t == 0) {
                    t = millis();
                }
            }
            if (t != 0 && cmd_args[2] > 0 && (millis() - t) >= cmd_args[2]) {
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

        pwm_set_all_duty_remapped(0, 0, 0, 0); // end
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
        int32_t x;
        bool res = eeprom_user_edit(str, &x);
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

int cmd_parseArgs(char* str)
{
    int i = 0;
    char * token = strtok(str, " ");
    while( token != NULL && i < MAX_CMD_ARGS)
    {
        if (i != 0)
        {
            int32_t x;
            if (token[0] == '0' && (token[1] == 'x' || token[1] == 'X'))
            {
                x = strtol(token, NULL, 16);
            }
            else
            {
                x = atoi();
            }
            cmd_args[i-1] = x;
        }
        token = strtok(NULL, " ");
        i += 1;
   }
   return 0;
}

void cli_reportSensors(Cereal* cer)
{
    cer->printf("[%u]: %0.0f, %0.0f, %0.0f, %u, \r\n", millis(), sensor_temperatureC, sensor_voltage, sensor_current, current_limit_val);
}
