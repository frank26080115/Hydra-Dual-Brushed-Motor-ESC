#include "main.h"
#include "version.h"

__attribute__((section(".firmware_info")))
const firmware_info_s firmware_info = {
    version_major  : VERSION_MAJOR,
    version_eeprom : VERSION_EEPROM,
    device_name    : "Hydra " FIRMWARE_NAME
};
