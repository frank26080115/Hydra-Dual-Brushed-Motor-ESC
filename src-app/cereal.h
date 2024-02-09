#pragma once

#include "conf.h"
#include "defs.h"
#include "types.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#ifdef ENABLE_COMPILE_CLI
#include <stdio.h>
#endif
#include "fifo.h"
#include "systick.h"
#include "mcu.h"
#include "debug_tools.h"
enum
{
    CEREAL_ID_USART1,
    CEREAL_ID_USART2,
    CEREAL_ID_USART_DEBUG,
    CEREAL_ID_USART_SWCLK,
    CEREAL_ID_USART_CRSF,
};

class Cereal
{
    public:
        virtual void write(uint8_t x);
        virtual size_t writeb(uint8_t* buf, int len);
        virtual void flush(void);
        virtual int16_t read(void);
        virtual int16_t peek(void);
        virtual int16_t peekAt(int16_t);
        virtual int16_t peekTail(void);
        virtual int available(void);
        virtual int read(uint8_t* buf, int len);
        virtual bool popUntil(uint8_t x);
        virtual int16_t consume(uint16_t);
        virtual void reset_buffer(void);
        virtual uint8_t* get_buffer(void);
        virtual uint32_t get_last_time(void);
        virtual bool get_idle_flag(bool clr);
        inline uint8_t get_id(void) { return _id; };

        #if defined(ENABLE_COMPILE_CLI) || defined(DEBUG_PRINT)
        size_t vprintf(const char *format, va_list arg);
        size_t printf(const char * format, ...) __attribute__ ((format (printf, 2, 3)));
        #endif

    protected:
        uint8_t _id; // only represents which serial port it is
        fifo_t* fifo_rx;
};

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t cer_buff_1[CEREAL_BUFFER_SIZE];
extern uint8_t cer_buff_2[CEREAL_BUFFER_SIZE];
//extern uint8_t cer_buff_3[CEREAL_BUFFER_SIZE];
//extern uint8_t cer_buff_4[CEREAL_BUFFER_SIZE];

#ifdef __cplusplus
}
#endif

class Cereal_USART : public Cereal
{
    public:
        Cereal_USART(void);
        void sw_init(uint8_t id);
        virtual void init(uint8_t id, uint32_t baud, bool halfdup, bool swap, bool dma = false, bool invert = false);
        virtual void write(uint8_t x);
        virtual void flush(void);
        virtual uint8_t* get_buffer(void);
        virtual uint32_t get_last_time(void);
        virtual bool get_idle_flag(bool clr);
        virtual void restart(void);
        #if defined(STMICRO)
        virtual void drain_echo(void);
        virtual int16_t read(void);
        virtual int16_t peek(void);
        virtual int16_t peekAt(int16_t);
        virtual int16_t peekTail(void);
        virtual int available(void);
        virtual int read(uint8_t* buf, int len);
        virtual bool popUntil(uint8_t x);
        virtual int16_t consume(uint16_t);
        #endif
    protected:
        #if defined(STMICRO)
            USART_TypeDef
        #elif defined(ARTERY)
            usart_type
        #endif
                        * _usart;
        uint8_t _u; // this always stores the usartx number no matter what `_id` is
        fifo_t* fifo_tx;

        // STM32's USART perpherial exhibits a echo problem while in half-duplex mode, does not affect AT32
        // https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/1
        bool fix_echo = false;
        uint32_t echo_time = 0;

        uint32_t cached_cr1;
        uint32_t cached_cr2;
        uint32_t cached_cr3;
};

#ifdef ENABLE_COMPILE_CLI

class Cereal_TimerBitbang : public Cereal
{
    // WARNING: this class does not seem to work on pin PA2, it only works on PB4

    public:
        Cereal_TimerBitbang(void);

        void sw_init(uint32_t baud);
        virtual void init(uint32_t baud);
        virtual void write(uint8_t x);
        virtual void flush(void);
        virtual uint32_t get_last_time(void);
        virtual bool get_idle_flag(bool clr);

    protected:
        fifo_t* fifo_tx;
};

#endif
