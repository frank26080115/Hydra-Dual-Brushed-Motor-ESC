#include "main.h"
#include "cereal.h"

#ifdef STMICRO
#include "cereal_timer.h"
#include "cereal_usart.h"
#endif

void cli_enter(void)
{
    #if INPUT_PIN == LL_GPIO_PIN_2
    Cereal_USART* cer = new Cereal_USART(2, CLI_BUFF_SIZE);
    cer->begin(CLI_BAUD, false, true, false);
    #elif INPUT_PIN == LL_GPIO_PIN_4
    Cereal_TimerBitbang* cer = new Cereal_TimerBitbang(0, CLI_BUFF_SIZE);
    cer->begin(CLI_BAUD);
    #endif

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
    }
}

void cli_execute(Cereal* cer, char* str)
{
    if (item_strcmp("list", str))
    {
        cer->printf("all settings:\r\n")
        eeprom_print_all(cer);
        cer->write('\r');
        cer->write('\n');
    }
    else if (item_strcmp("hwdebug", str))
    {
        cli_hwdebug ^= true;
        cer->printf("hardware debug: %u\r\n", cli_hwdebug);
    }
}
