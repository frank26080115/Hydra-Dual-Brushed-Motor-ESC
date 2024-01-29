#pragma once

#include "cereal.h"
#include "stm32.h"
#include "debug_tools.h"

enum
{
    CEREAL_ID_USART1,
    CEREAL_ID_USART2,
    CEREAL_ID_USART_DEBUG,
    CEREAL_ID_USART_SWCLK,
    CEREAL_ID_USART_CRSF,
};

class Cereal_USART : public Cereal
{
    public:
        Cereal_USART(void);
        virtual void init(uint8_t id, uint32_t baud, bool invert, bool halfdup, bool swap, bool dma = false);
        virtual void write(uint8_t x);
        virtual void flush(void);
        virtual uint8_t* get_buffer(void);
        virtual uint32_t get_last_time(void);
        virtual bool get_idle_flag(bool clr);
    protected:
        USART_TypeDef* _usart;
        uint8_t _u;
        fifo_t* fifo_tx;
};
