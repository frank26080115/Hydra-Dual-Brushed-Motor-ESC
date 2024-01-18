#pragma once

#include "conf.h"
#include "defs.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    VSPLITMODE_HALF_VOLTAGE,
    VSPLITMODE_BOOST_ALWAYS,
    VSPLITMODE_BOOST_FORWARD,
};

enum
{
    INPUTMODE_RC = 0x01,
    INPUTMODE_CRSF = 0x02,
    // TODO: support SBUS and IBUS
};

#define EEPROM_chksum_t uint16_t

typedef struct
{
    uint8_t fool_am32_bootloader_0; // the first few bytes are being checked by the AM32 bootloader, it needs to be a specific value to boot
    uint8_t fool_am32_bootloader_1;
    uint8_t fool_am32_bootloader_2;
    uint8_t fool_am32_eeprom_layout;
    uint8_t fool_am32_version_major;
    uint8_t fool_am32_version_minor;
    uint8_t fool_am32_name[12];

    // start defining the actual EEPROM

    uint32_t magic;
    uint8_t version_major;
    uint8_t version_eeprom;

    uint8_t voltage_split_mode;
    uint8_t input_mode;
    uint8_t phase_map;
    uint32_t baud;

    // CRSF channel selection
    uint8_t channel_0;
    uint8_t channel_1;
    uint8_t channel_mode;

    // RC signal calibration, units are microseconds
    uint16_t rc_mid;
    uint16_t rc_range;
    uint16_t rc_deadzone;

    // driver PWM options
    uint32_t pwm_reload;
    uint32_t pwm_headroom;

    bool braking;
    bool chan_swap;
    bool flip_0;
    bool flip_1;
    bool tied;
    uint16_t arm_duration;
    uint8_t temperature_limit;
    uint32_t current_limit;

    EEPROM_chksum_t chksum;
} __attribute__((packed))
EEPROM_data_t;

typedef struct
{
    char     name[32];
    uint32_t ptr;
    uint8_t  size;
} __attribute__((packed))
EEPROM_item_t;

#ifdef __cplusplus
}
#endif
