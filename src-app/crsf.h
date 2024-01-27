#pragma once

#include "rc.h"
#include "cereal.h"

// these CRSF_CHANNEL_VALUE_ constants are taken from https://github.com/ExpressLRS/ExpressLRS/blob/master/src/lib/CrsfProtocol/crsf_protocol.h 
#define CRSF_CHANNEL_VALUE_MIN  172 // 987us - actual CRSF min is 0 with E.Limits on
#define CRSF_CHANNEL_VALUE_1000 191
#define CRSF_CHANNEL_VALUE_MID  992
#define CRSF_CHANNEL_VALUE_2000 1792
#define CRSF_CHANNEL_VALUE_MAX  1811 // 2012us - actual CRSF max is 1984 with E.Limits on

#define CRSF_BAUDRATE 420000

class CrsfChannel : public RcChannel
{
    public:
        CrsfChannel(void);
        virtual void init(Cereal* cer, uint8_t idx);
        virtual void task(void);
        virtual int16_t read(void);
        virtual int16_t readRaw(void);
        virtual bool is_alive(void);
        virtual bool has_new(bool clr);
        virtual bool is_armed(void);
        virtual void disarm(void);
        #ifdef RC_LOG_JITTER
        virtual uint32_t readJitter(void); // reads 0 for CRSF, any noise can be measured from the transmitter
        #endif
    protected:
        uint8_t _idx;
        uint32_t arming_cnt = 0;
        bool armed = false;
        bool _has_new = false;
        uint32_t arming_tick = 0;
};

uint16_t crsf_readChan(uint8_t i);
