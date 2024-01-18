#pragma once

#include "main.h"
#include "cereal.h"

class RcChannel
{
    public:
        virtual void init(void);
        virtual void task(void);
        inline uint16_t read(void) { return val; }
    protected:
        uint16_t val_raw;
        uint16_t val;
};

class RcPulse : public RcChannel
{

};

class RcBusChannel : public RcChannel
{
    public:
        RcBusChannel(uint8_t chan_idx, Cereal* cer);

    protected:
        Cereal* _cer;
};

extern uint16_t rc_pulse_map(uint16_t x);
