#include "cereal.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t cer_buff_1[CEREAL_BUFFER_SIZE];
uint8_t cer_buff_2[CEREAL_BUFFER_SIZE];
//uint8_t cer_buff_3[CEREAL_BUFFER_SIZE];
//uint8_t cer_buff_4[CEREAL_BUFFER_SIZE];

#if defined(ENABLE_COMPILE_CLI) || defined(DEBUG_PRINT)
uint8_t cer_buff_5[CEREAL_BUFFER_SIZE];
#endif

#ifdef __cplusplus
}
#endif

#if defined(ENABLE_COMPILE_CLI) || defined(DEBUG_PRINT)

size_t Cereal::vprintf(const char *format, va_list arg)
{
    // got this code from https://github.com/espressif/arduino-esp32/blob/826a426905191a246c96f21ba80372ec8324c59a/cores/esp32/Print.cpp
    char* loc_buf = (char*)cer_buff_5;
    char * temp = loc_buf;
    va_list copy;
    va_copy(copy, arg);
    int len = vsnprintf(temp, CEREAL_BUFFER_SIZE, format, copy);
    va_end(copy);
    if (len < 0) {
        va_end(arg);
        return 0;
    }
    if (len >= CEREAL_BUFFER_SIZE) {
        va_end(arg);
        return 0;
    }
    va_end(arg);
    len = writeb((uint8_t*)temp, len);

    return len;
}

size_t Cereal::printf(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    size_t ret = this->vprintf(format, arg);
    va_end(arg);
    return ret;
}

#endif

size_t Cereal::writeb(uint8_t* buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        write(buf[i]);
    }
    return len;
}

void Cereal::write(uint8_t x) {
    // dummy, must be overridden
}

void Cereal::flush(void) {
    // dummy, must be overridden
}

int16_t Cereal::read(void)
{
    return fifo_pop(fifo_rx);
}

int16_t Cereal::peek(void)
{
    return fifo_peek(fifo_rx);
}

int16_t Cereal::peekAt(int16_t i)
{
    return fifo_peekAt(fifo_rx, i);
}

int16_t Cereal::peekTail(void)
{
    return fifo_peekTail(fifo_rx);
}

int16_t Cereal::consume(uint16_t x)
{
    return fifo_consume(fifo_rx, x);
}

int Cereal::available(void)
{
    return fifo_available(fifo_rx);
}

int Cereal::read(uint8_t* buf, int len)
{
    return fifo_readN(fifo_rx, buf, len);
}

bool Cereal::popUntil(uint8_t x)
{
    return fifo_popUntil(fifo_rx, x);
}

void Cereal::reset_buffer(void)
{
    fifo_reset(fifo_rx);
}

uint8_t* Cereal::get_buffer(void)
{
    return fifo_rx->buf;
}

uint32_t Cereal::get_last_time(void)
{
    return 0;
}

bool Cereal::get_idle_flag(bool clr)
{
    return false;
}
