#include "cereal.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t cer_buff_1[CEREAL_BUFFER_SIZE];
uint8_t cer_buff_2[CEREAL_BUFFER_SIZE];

#if defined(ENABLE_COMPILE_CLI) || defined(DEBUG_PRINT)
uint8_t cer_buff_3[CEREAL_BUFFER_SIZE];
#endif

#ifdef __cplusplus
}
#endif



#ifdef ENABLE_COMPILE_CLI

size_t Cereal::printf(const char *format, ...)
{
    char* loc_buf = (char*)cer_buff_3;
    char * temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, CEREAL_BUFFER_SIZE, format, copy);
    va_end(copy);
    if (len < 0) {
        va_end(arg);
        return 0;
    }
    if (len >= CEREAL_BUFFER_SIZE) {
        #if 0
        temp = (char*) malloc(len+1);
        if(temp == NULL) {
            va_end(arg);
            return 0;
        }
        len = vsnprintf(temp, len+1, format, arg);
        #else
        va_end(arg);
        return 0;
        #endif
    }
    va_end(arg);
    len = write((uint8_t*)temp, len);
    #if 0
    if (temp != loc_buf){
        free(temp);
    }
    #endif
    return len;
}
#endif

#ifdef ENABLE_CEREAL_TX
size_t Cereal::write(uint8_t* buf, int len)
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

#endif

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
    int i;
    for (i = 0; i < len; i++)
    {
        int16_t c = fifo_pop(fifo_rx);
        if (c < 0) {
            return i;
        }
        buf[i] = c;
    }
    return i;
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
