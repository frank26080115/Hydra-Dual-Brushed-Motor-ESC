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
    VSPLITMODE_END,
};

enum
{
    INPUTMODE_RC,
    INPUTMODE_CRSF,
    INPUTMODE_RC_SWD,     // PA13 or PA14
    INPUTMODE_CRSF_SWCLK, // PA14

    // TODO: support SBUS and IBUS
};

#define EEPROM_chksum_t uint8_t

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
    uint8_t version_minor;
    uint8_t version_eeprom;

    uint8_t voltage_split_mode;
    bool load_balance;
    uint8_t input_mode;
    uint8_t phase_map;         // 1 to 3, which phase is the common-shared phase, 0 means 1, overflow is handled by modulo
    uint32_t baud;             // 0 means automatic, otherwise it will override CRSF baudrate

    // hardware ADC calibration
    uint32_t voltage_divider;  // default uses target hardware definition
    uint32_t current_offset;   // default uses target hardware definition
    uint32_t current_scale;    // default uses target hardware definition
    uint16_t adc_filter;       // 0 to 1000, 100 meaning 10% of new-value and 90% of old-value

    // CRSF channel selection
    uint8_t channel_1;
    uint8_t channel_2;
    uint8_t channel_mode;
    // use 0 if not used

    // RC signal calibration, units are microseconds
    uint16_t rc_mid;       // do not apply to CRSF data
    uint16_t rc_range;     // do not apply to CRSF data
    uint16_t rc_deadzone;  // does apply to CRSF data

    // driver PWM options
    uint32_t pwm_reload;
    uint32_t pwm_headroom;

    bool braking;
    bool chan_swap;
    bool flip_1;
    bool flip_2;
    bool tied;
    uint32_t arm_duration;
    uint32_t disarm_timeout;
    uint8_t temperature_limit;
    uint32_t current_limit;
    uint32_t voltage_limit;

    int32_t currlim_kp;
    int32_t currlim_ki;
    int32_t currlim_kd;

    uint32_t useless; // just a test pattern that can be written and used to change the chksum

    EEPROM_chksum_t chksum;

    // any data after the checksum can still be saved but won't be protected

    uint8_t tone_volume;

} __attribute__((packed)) __attribute__((aligned(4)))
EEPROM_data_t;

// to keep the code simple, all EEPROM elements that need to be signed must also be 32 bits

typedef struct
{
    char     name[32];
    uint32_t ptr;
    uint8_t  size;
} __attribute__((packed)) __attribute__((aligned(4)))
EEPROM_item_t;

typedef struct {
    int32_t error;
    int32_t Kp;
    int32_t Ki;
    int32_t Kd;
    int32_t integral;
    int32_t derivative;
    int32_t last_error;
    int32_t pid_output;
    int32_t integral_limit;
    int32_t output_limit;
} __attribute__((aligned(4)))
pidloop_t;

typedef struct
{
    uint32_t device_code;     // this is metadata that the installer app looks at to verify if the bootloader matches the firmware file
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t version_eeprom;
    char device_name[33];     // size must match allocation in linker script
    // size is 40
}
__attribute__((packed))
firmware_info_s;

#ifdef __cplusplus
}
#endif
