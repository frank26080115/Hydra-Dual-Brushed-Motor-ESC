#include "eeprom.h"
#include "targets.h"
#include "stm32.h"

#if defined(MCU_F051) || defined(MCU_F031)
#define EEPROM_PAGE_SIZE 0x400
#define FLASH_FKEY1      (uint32_t)0x45670123
#define FLASH_FKEY2      (uint32_t)0xCDEF89AB
#define WRITE_SIZE_TYPE  uint16_t
#endif

#ifdef MCU_G071
#define EEPROM_PAGE_SIZE 0x800
#define FLASH_FKEY1      (uint32_t)0x45670123
#define FLASH_FKEY2      (uint32_t)0xCDEF89AB
#define WRITE_SIZE_TYPE  uint32_t
#define EEPROM_WRITE_TWO_CHUNKS
#define FLASH_SR_BSY     FLASH_SR_BSY1
#endif

#ifdef MCU_GD32F350
#define EEPROM_PAGE_SIZE 0x800
#define FLASH_FKEY1      (uint32_t)0x45670123
#define FLASH_FKEY2      (uint32_t)0xCDEF89AB
#define WRITE_SIZE_TYPE  uint16_t
#endif

void eeprom_write(uint32_t* data, int len, uint32_t addr)
{
    #ifdef DISABLE_EEPROM
    volatile char x = 0; x = 1; if (x) { return; }
    // this makes sure the function is still compiled, but never runs
    #endif

    // make sure none of the bytes are missing after integer division
    while ((len % sizeof(WRITE_SIZE_TYPE)) != 0) {
        len++;
    }

    // This must be 32-bit aligned! That means the pointer being passed in must also be 32-bit aligned!
    // Or else it will hard-fault!
    WRITE_SIZE_TYPE* ptr_x = (WRITE_SIZE_TYPE*)data;

    while ((FLASH->SR & FLASH_SR_BSY) != 0) {
        // do nothing wait while busy
    }

    // unlock flash
    if ((FLASH->CR & FLASH_CR_LOCK) != 0) {
        FLASH->KEYR = FLASH_FKEY1;
        FLASH->KEYR = FLASH_FKEY2;
    }

    // erase page
    FLASH->CR |= FLASH_CR_PER;
    #if defined(MCU_F051) || defined(MCU_F031) || defined(MCU_GD32F350)
    FLASH->AR = addr;
    #elif defined(MCU_G071)
    FLASH->CR |= (addr / EEPROM_PAGE_SIZE) << 3;
    #endif
    FLASH->CR |= FLASH_CR_STRT;
    while ((FLASH->SR & FLASH_SR_BSY) != 0) {
        // do nothing wait while busy
    }
    if ((FLASH->SR & FLASH_SR_EOP) != 0) {
        FLASH->SR = FLASH_SR_EOP;
    }
    else {
        // ERROR
    }
    FLASH->CR &= ~FLASH_CR_PER;

    int i;
    for (i = 0; i < len / sizeof(WRITE_SIZE_TYPE); i++)
    {
        FLASH->CR |= FLASH_CR_PG; // prep for data write
        *(__IO WRITE_SIZE_TYPE*)(addr + (i * sizeof(WRITE_SIZE_TYPE))) = ptr_x[i];
        #ifdef EEPROM_WRITE_TWO_CHUNKS
        i += 1;
        *(__IO WRITE_SIZE_TYPE*)(addr + (i * sizeof(WRITE_SIZE_TYPE))) = ptr_x[i];
        #endif

        while ((FLASH->SR & FLASH_SR_BSY) != 0) {
            // do nothing wait while busy
        }

        if ((FLASH->SR & FLASH_SR_EOP) != 0) {
            FLASH->SR = FLASH_SR_EOP;
        }
        else {
            // ERROR
        }
        FLASH->CR &= ~FLASH_CR_PG;
    }

    SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}
