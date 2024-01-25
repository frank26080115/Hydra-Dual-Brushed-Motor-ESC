#pragma once

// this file is used for preprocessor configurations

#define HW_TESTS

#define EEPROM_DIRTY_SAVE_TIME_MS 1000 // if the user edits a EEPROM item, it will auto-save only after this amount of time

#define CLI_ENTER_LOW_CRITERIA  10000 // user must leave ESC disconnected for 10 seconds to enter CLI
#define CLI_ENTER_HIGH_CRITERIA 5000  // after the first 10 seconds, the user must plug in the USB linker for an additional 5 seconds

#define THROTTLE_UNIT_RANGE 1000 // scales all inputs to be in the range between -1000 and 1000

#define CEREAL_BUFFER_SIZE 128 // malloc is too big to compile so we have a few static buffers being used

#define CLI_BAUD      19200 // baud rate used for CLI
#define CLI_BUFF_SIZE 64
#define CLI_ECHO      false // most of the one-wire USB-links already echo, so do not echo ourselves

#define PWM_DEFAULT_HEADROOM     50 // for the high-MOSFET's charge pump

#define ADC_FILTER_DEFAULT  100     // 0 to 1000, 100 meaning 10% of new-value and 90% of old-value

#define RC_INPUT_TIMEOUT    200     // this isn't really actually used much
#define RC_INPUT_VALID_MIN  800     // criteria for a valid pulse
#define RC_INPUT_VALID_MAX  2200    // criteria for a valid pulse
#define RC_DISARM_TIMEOUT   3000    // this can be adjusted in EEPROM
#define RC_ARMING_CNT_REQ   10      // this can be adjusted in EEPROM

#define TEMPERATURE_OVER    20      // if the thermal limit is set at 80, then 80 is when the power starts to drop, and 100 is when power is completly off
#define UNDERVOLTAGE        100     // if the voltage limit is set at 3000, then 3000 is when the power starts to drop, and 2900 is when power is completly off
