#pragma once

#include "main.h"
#include "userconfig.h"
#include "systick.h"

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
        #ifdef RC_LOG_JITTER
        virtual uint32_t readJitter(void);
        #endif
};

extern uint32_t arm_pulses_required;       // copy user config data into this variable
extern uint32_t disarm_timeout;            // copy user config data into this variable

extern int16_t rc_pulse_map(uint16_t x);   // outputs normalized signed value, already accounting for deadzone

bool rc_find_arming_vals(int32_t multi, int32_t offset, uint16_t* out_min, uint16_t* out_max); // calculates arming values for speeding up the ISRs

void rc_register_good_pulse(uint16_t pw, uint16_t arm_thresh_min, uint16_t arm_thresh_max, uint32_t* timestamp, uint8_t* good_cnt, uint8_t* bad_cnt, uint32_t* arm_cnt, bool* nflag, bool* armed);
void rc_register_bad_pulse(uint8_t* good_cnt, uint8_t* bad_cnt, uint32_t* arm_cnt);

#include "mcu.h"

#if defined(STMICRO)
#include "rc_stm32.h"
#elif defined(ARTERY)
#include "rc_at32.h"
#endif
