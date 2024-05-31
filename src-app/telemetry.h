#pragma once

#include "cereal.h"

enum
{
    TELEMPIN_NO_NEED,
    TELEMPIN_PA2_WAS_CRSF   = 0x01,
    TELEMPIN_PA2_WAS_PWM    = 0x02,
    TELEMPIN_PB4_WAS_PWM    = 0, // do nothing
    TELEMPIN_PB6_WAS_CRSF   = 0x04,
    TELEMPIN_PB6_WAS_PWM    = 0x08,
    TELEMPIN_PA14_WAS_CRSF  = 0x10,
    TELEMPIN_PA14_WAS_PWM   = 0x20,
    TELEMPIN_PB6_TO_TELEM   = 0x40,
    TELEMPIN_PA14_TO_TELEM  = 0x80,
};

void telemetry_init(void);
void telem_crsfTask(void); // call this immediately after reception of a CRSF packet
void telem_mainTask(void); // call this from the main loop
