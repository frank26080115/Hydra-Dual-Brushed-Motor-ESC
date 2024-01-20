#pragma once

#include "cereal.h"
#include "stm32.h"

class Cereal_TimerBitbang : public Cereal
{
    public:
        Cereal_TimerBitbang(uint8_t id, uint16_t sz);

        virtual void begin(uint32_t baud);
        virtual void write(uint8_t x);
        virtual void flush(void);
        virtual uint32_t get_last_time(void);
        virtual bool get_idle_flag(bool clr);

    protected:
        uint16_t _buff_size;
        fifo_t* fifo_tx;
};
