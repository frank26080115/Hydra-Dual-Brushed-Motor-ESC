#pragma once

#include "rc.h"
#include "cereal.h"

class CrsfChannel : public RcChannel
{
    public:
        CrsfChannel(Cereal* cer, uint8_t idx);
        virtual void init(void);
        virtual void task(void);
        virtual int16_t read(void);
        virtual bool is_alive(void);
        virtual bool has_new(bool clr);
    protected:
        uint8_t _idx;
};
