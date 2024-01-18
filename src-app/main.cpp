#include "main.h"
#include "userconfig.h"

int main(void)
{
    mcu_init();

    eeprom_load_or_default();

    return 0;
}
