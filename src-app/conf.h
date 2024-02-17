#pragma once

// this file is used for preprocessor configurations

//#define HW_TESTS
//#define RC_LOG_JITTER
//#define DISABLE_EEPROM
//#define ENABLE_TONE

//#define DEBUG_PINTOGGLE

//#define SIMULATE_CURRENT_LIMIT
//#define SIMULATE_VOLTAGE_LIMIT
//#define SIMULATE_TEMPERATURE_LIMIT

#define EEPROM_DIRTY_SAVE_TIME_MS 200 // if the user edits a EEPROM item, it will auto-save only after this amount of time

#define CLI_ENTER_UNPLUG_CRITERIA 10 // user must leave ESC disconnected for 10 seconds to enter CLI

#define THROTTLE_UNIT_RANGE 1000 // scales all inputs to be in the range between -1000 and 1000

#define CEREAL_BUFFER_SIZE 128 // malloc is too big to compile so we have a few static buffers being used

#define CLI_BAUD      19200 // baud rate used for CLI
#define CLI_BUFF_SIZE 64

#define CRSF_BAUDRATE 420000

#define PWM_DEFAULT_DEADTIME     50 // for the high-MOSFET's charge pump

#define ADC_FILTER_DEFAULT  10      // 0 to 1000, 100 meaning 10% of new-value and 90% of old-value

#define RC_INPUT_TIMEOUT    200     // this isn't really actually used much
#define RC_INPUT_VALID_MIN  800     // criteria for a valid pulse
#define RC_INPUT_VALID_MAX  2200    // criteria for a valid pulse
#define RC_DISARM_TIMEOUT   3000    // this can be adjusted in EEPROM
#define RC_ARMING_CNT_REQ   10      // this can be adjusted in EEPROM

#define TEMPERATURE_OVER    20      // if the thermal limit is set at 80, then 80 is when the power starts to drop, and 100 is when power is completely off
#define UNDERVOLTAGE        100     // if the voltage limit is set at 3000, then 3000 is when the power starts to drop, and 2900 is when power is completely off
// `UNDERVOLTAGE` will be overridden if the user specifies `lowbattstretch`

#define BATTERY_RISE_MAX_TIME 3000  // time in milliseconds it takes before battery cell count takes place

#define TONE_DEF_VOLUME     80      // range 0 to 100
#define PWM_TONE_RELOAD     (PWM_DEFAULT_DEADTIME * 40) // want a faster reload for tone generation
