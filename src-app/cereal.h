#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "fifo.h"
#include "systick.h"

class Cereal
{
    public:
        virtual void write(uint8_t x);
        virtual size_t write(uint8_t* buf, int len);
        virtual void flush(void);
        virtual int16_t read(void);
        virtual int16_t peek(void);
        virtual int16_t peekAt(int16_t);
        virtual int16_t peekTail(void);
        virtual int available(void);
        virtual int read(uint8_t* buf, int len);
        virtual int16_t consume(uint16_t);
        virtual void reset_buffer(void);
        virtual uint8_t* get_buffer(void);
        virtual uint32_t get_last_time(void);
        virtual bool get_idle_flag(bool clr);
        inline uint8_t get_id(void) { return _id; };
        inline void set_echo(bool x) { _echo = x; };

        #ifdef ENABLE_COMPILE_CLI
        size_t printf(const char * format, ...) __attribute__ ((format (printf, 2, 3)));
        #endif

    protected:
        uint8_t _id; // only represents which serial port it is
        fifo_t* fifo_rx;
        bool _echo;
};
