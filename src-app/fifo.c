#include "fifo.h"

void fifo_init(fifo_t* f, void* buff, uint16_t sz)
{
    f->buf = (uint8_t*)buff;
    f->size = sz;
    f->pread = 0;
    f->pwrite = 0;
}

void fifo_reset(fifo_t* f)
{
    FIFO_DISABLE_IRQ();
    f->pread = 0;
    f->pwrite = 0;
    FIFO_ENABLE_IRQ();
}

int16_t fifo_peek(fifo_t* f)
{
    FIFO_DISABLE_IRQ();
    if (f->pread == f->pwrite) {
        FIFO_ENABLE_IRQ();
        return -1;
    }
    volatile int16_t x = f->buf[f->pread];
    FIFO_ENABLE_IRQ();
    return x;
}

int16_t fifo_peekAt(fifo_t* f, int16_t i)
{
    FIFO_DISABLE_IRQ();
    uint16_t x = f->pwrite + f->size;
    volatile int16_t d = -1;
    x -= f->pread;
    x %= f->size;
    if (i < x)
    {
        x = f->pread;
        x += i;
        x %= f->size;
        d = f->buf[x];
    }
    FIFO_ENABLE_IRQ();
    return d;
}

int16_t fifo_peekTail(fifo_t* f)
{
    FIFO_DISABLE_IRQ();
    if (f->pread == f->pwrite) {
        FIFO_ENABLE_IRQ();
        return -1;
    }
    uint16_t i = f->pwrite;
    if (i <= 0) {
        i = f->size -1;
    }
    volatile int16_t x = f->buf[i];
    FIFO_ENABLE_IRQ();
    return x;
}

int16_t fifo_pop(fifo_t* f)
{
    FIFO_DISABLE_IRQ();
    if (f->pread == f->pwrite) {
        FIFO_ENABLE_IRQ();
        return -1;
    }
    uint8_t x = f->buf[f->pread];
    f->pread += 1;
    f->pread %= f->size;
    FIFO_ENABLE_IRQ();
    return (int16_t)x;
}

int fifo_readN(fifo_t* f, uint8_t* dst, uint16_t len)
{
    #if 0
    FIFO_DISABLE_IRQ();

    uint16_t i;
    for (i = 0; i < len; i++) {
        if (f->pread == f->pwrite) {
            break;
        }
        dst[i] = f->buf[f->pread];
        f->pread += 1;
        f->pread %= f->size;
    }

    FIFO_ENABLE_IRQ();
    return i;
    #else
    uint16_t i;
    int16_t c;
    for (i = 0; i < len; i++)
    {
        c = fifo_pop(f);
        if (c < 0) {
            break;
        }
        dst[i] = c;
    }
    return i;
    #endif
}

bool fifo_popUntil(fifo_t* f, uint8_t x)
{
    #if 0
    FIFO_DISABLE_IRQ();

    while (true)
    {
        if (f->pread == f->pwrite) {
            break;
        }
        uint8_t d = f->buf[f->pread];
        if (d == x) {
            FIFO_ENABLE_IRQ();
            return true;
        }
        f->pread += 1;
        f->pread %= f->size;
    }

    FIFO_ENABLE_IRQ();
    return false;
    #else
    while (true)
    {
        int16_t d = fifo_peek(f);
        if (d == x) {
            return true;
        }
        else if (d < 0) {
            return false;
        }
        fifo_pop(f);
    }
    #endif
}

void fifo_push(fifo_t* f, uint8_t x)
{
    FIFO_DISABLE_IRQ();
    uint16_t next = (f->pwrite + 1) % f->size;
    if (next != f->pread) {
        f->buf[f->pwrite] = x;
        f->pwrite = next;
    }
    FIFO_ENABLE_IRQ();
}

int fifo_available(fifo_t* f) {
    FIFO_DISABLE_IRQ();
    uint32_t x = f->pwrite + f->size;
    x -= f->pread;
    x %= f->size;
    FIFO_ENABLE_IRQ();
    return x;
}

int16_t fifo_consume(fifo_t* f, uint16_t x)
{
    uint16_t avail = fifo_available(f);
    FIFO_DISABLE_IRQ();
    x = x > avail ? avail : x;
    f->pread += x;
    f->pread %= f->size;
    FIFO_ENABLE_IRQ();
    return x;
}
