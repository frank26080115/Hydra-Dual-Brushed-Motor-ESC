#pragma once

#include <stdint.h>
#include "fifo.h"

class Cereal
{
    public:
        Cereal(uint8_t id);
        virtual void begin(uint32_t baud);
        virtual void write(uint8_t x);
        virtual void write(uint8_t* buf, int len);
        virtual int16_t read(void);
        virtual int16_t peek(void);
        virtual int available(void);
        virtual int read(uint8_t* buf, int len);

    private:
        uint8_t _id; // only represents which serial port it is
}
