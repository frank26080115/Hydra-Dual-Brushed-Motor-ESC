#pragma once

#include "main.h"
#include "cereal.h"

class RcChannel
{
    public:
        virtual void init(void);
        virtual void task(void);
        virtual uint16_t read(void);
        virtual bool is_alive(void);
        virtual bool has_new(bool clr);
};

class RcBusChannel : public RcChannel
{
    public:
        RcBusChannel(uint8_t chan_idx, Cereal* cer);
        virtual void init(void);
        virtual void task(void);
        virtual uint16_t read(void);
        virtual bool is_alive(void);
        virtual bool has_new(bool clr);

    protected:
        Cereal* _cer;
};

extern int16_t rc_pulse_map(uint16_t x);
