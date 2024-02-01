#include "main.h"
#include "mcu.h"
#include "version.h"

__attribute__((section(".firmware_info")))
const firmware_info_s firmware_info = {
    version_major  : VERSION_MAJOR,
    version_eeprom : VERSION_EEPROM,
    device_code    : // this is metadata that the installer app looks at to verify if the bootloader matches the firmware file
        ((((uint32_t)INPUT_PIN_PORT) >> 8) & 0xFF)
        | (
        #if INPUT_PIN == (1 << 0)
            0
        #elif INPUT_PIN == (1 << 1)
            1
        #elif INPUT_PIN == (1 << 2)
            2
        #elif INPUT_PIN == (1 << 3)
            3
        #elif INPUT_PIN == (1 << 4)
            4
        #elif INPUT_PIN == (1 << 5)
            5
        #elif INPUT_PIN == (1 << 6)
            6
        #elif INPUT_PIN == (1 << 7)
            7
        #elif INPUT_PIN == (1 << 8)
            8
        #elif INPUT_PIN == (1 << 9)
            9
        #elif INPUT_PIN == (1 << 10)
            10
        #elif INPUT_PIN == (1 << 11)
            11
        #elif INPUT_PIN == (1 << 12)
            12
        #elif INPUT_PIN == (1 << 13)
            13
        #elif INPUT_PIN == (1 << 14)
            14
        #elif INPUT_PIN == (1 << 15)
            15
        #else
            #error
        #endif
        << 8)
        |
        #if defined(MCU_F051)
        (0x51 << 16)
        #elif defined(MCU_G071)
        (0x71 << 16)
        #elif defined(MCU_F421)
        (0x21 << 16)
        #else
        #error
        #endif
        ,

    device_name    : "Hydra " FIRMWARE_NAME
};
