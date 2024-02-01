#include "eeprom.h"
#include "targets.h"
#include "at32.h"
#include "at32f421_flash.h"

#if defined(MCU_AT421)
#define EEPROM_PAGE_SIZE 0x400
#define WRITE_SIZE_TYPE  uint32_t
#endif

void eeprom_write(uint8_t* data, int len, uint32_t addr)
{
    #ifdef DISABLE_EEPROM
    volatile char  x = 0;
    x = 1;
    if (x) {
        return;
    }
    #endif

    WRITE_SIZE_TYPE* ptr_x = (WRITE_SIZE_TYPE*)data;

    flash_unlock();
    flash_sector_erase(addr);
    uint32_t i;
    for (i = 0; i < len / sizeof(WRITE_SIZE_TYPE); i++)
    {
        flash_word_program(addr, ptr_x[i]);
        flash_flag_clear(FLASH_PROGRAM_ERROR | FLASH_EPP_ERROR | FLASH_OPERATE_DONE);
        addr += sizeof(WRITE_SIZE_TYPE);
    }
    flash_lock();
}
