#include "main.h"
#include "mcu.h"
#include "version.h"

__attribute__((section(".firmware_info")))
const firmware_info_s firmware_info = {
    version_major  : VERSION_MAJOR,
    version_eeprom : VERSION_EEPROM,
    device_code    :
        ((((uint32_t)INPUT_PIN_PORT) >> 8) & 0xFF)
        | (
        #if INPUT_PIN == LL_GPIO_PIN_0
            0
        #elif INPUT_PIN == LL_GPIO_PIN_1
            1
        #elif INPUT_PIN == LL_GPIO_PIN_2
            2
        #elif INPUT_PIN == LL_GPIO_PIN_3
            3
        #elif INPUT_PIN == LL_GPIO_PIN_4
            4
        #elif INPUT_PIN == LL_GPIO_PIN_5
            5
        #elif INPUT_PIN == LL_GPIO_PIN_6
            6
        #elif INPUT_PIN == LL_GPIO_PIN_7
            7
        #elif INPUT_PIN == LL_GPIO_PIN_8
            8
        #elif INPUT_PIN == LL_GPIO_PIN_9
            9
        #elif INPUT_PIN == LL_GPIO_PIN_10
            10
        #elif INPUT_PIN == LL_GPIO_PIN_11
            11
        #elif INPUT_PIN == LL_GPIO_PIN_12
            12
        #elif INPUT_PIN == LL_GPIO_PIN_13
            13
        #elif INPUT_PIN == LL_GPIO_PIN_14
            14
        #elif INPUT_PIN == LL_GPIO_PIN_15
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
        #else
        #error
        #endif
        ,

    device_name    : "Hydra " FIRMWARE_NAME
};
