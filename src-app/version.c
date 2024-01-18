#include "main.h"

typedef struct __attribute__((packed)) {
    uint8_t version_major;
    uint8_t version_eeprom;
    char device_name[12];
} firmware_info_s;

firmware_info_s __attribute__((section(".firmware_info"))) firmware_info = {
    version_major  : VERSION_MAJOR,
    version_eeprom : VERSION_MINOR,
    device_name    : FIRMWARE_NAME
};
