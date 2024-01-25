#pragma once

#include "main.h"
#include "cereal.h"

class RcChannel
{
    public:
        virtual void init(void);
        virtual void task(void);           // called periodically for house keeping
        virtual int16_t read(void);        // outputs normalized signed value, already accounting for deadzone
        virtual int16_t readRaw(void);     // reads raw value for debugging
        virtual bool is_alive(void);       // used to check if wire is unplugged or receiver is off
        virtual bool has_new(bool clr);
        virtual bool is_armed(void);
        virtual void disarm(void);
};

extern uint32_t arm_pulses_required;       // copy user config data into this variable
extern uint32_t disarm_timeout;            // copy user config data into this variable

extern int16_t rc_pulse_map(uint16_t x);   // outputs normalized signed value, already accounting for deadzone
