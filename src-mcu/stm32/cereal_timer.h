#pragma once

#include "cereal.h"
#include "stm32.h"
#include "debug_tools.h"

#ifdef ENABLE_COMPILE_CLI

class Cereal_TimerBitbang : public Cereal
{
    public:
        Cereal_TimerBitbang(uint8_t id);

        virtual void init(uint32_t baud);
        #ifdef ENABLE_CEREAL_TX
        virtual void write(uint8_t x);
        virtual void flush(void);
        #endif
        virtual uint32_t get_last_time(void);
        #ifdef ENABLE_CEREAL_IDLE_DETECT
        virtual bool get_idle_flag(bool clr);
        #endif

    protected:
        #ifdef ENABLE_CEREAL_TX
        fifo_t* fifo_tx;
        #endif
};

#endif
