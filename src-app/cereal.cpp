#include "cereal.h"

size_t Cereal::printf(const char *format, ...)
{
    char loc_buf[64];
    char * temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    int len = vsnprintf(temp, sizeof(loc_buf), format, copy);
    va_end(copy);
    if(len < 0) {
        va_end(arg);
        return 0;
    }
    if(len >= (int)sizeof(loc_buf)){  // comparation of same sign type for the compiler
        temp = (char*) malloc(len+1);
        if(temp == NULL) {
            va_end(arg);
            return 0;
        }
        len = vsnprintf(temp, len+1, format, arg);
    }
    va_end(arg);
    len = write((uint8_t*)temp, len);
    if(temp != loc_buf){
        free(temp);
    }
    return len;
}

size_t Cereal::write(uint8_t* buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        write(buf[i]);
    }
    return len;
}

int16_t Cereal::read(void)
{
    return fifo_pop(fifo_rx);
}

int16_t Cereal::peek(void)
{
    return fifo_peek(fifo_rx);
}

int16_t Cereal::peektail(void)
{
    return fifo_peek(fifo_rx);
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
        int16_t c = fifo_read(fifo_rx);
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
