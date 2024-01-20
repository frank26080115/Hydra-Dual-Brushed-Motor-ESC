#include "fifo.h"

fifo_t* fifo_init(uint16_t sz)
{
    fifo_t* f = (fifo_t*)malloc(sizeof(fifo_t));
    f->buf = (uint8_t*)malloc((size_t)sz);
    f->size = sz;
    f->pread = 0;
    f->pwrite = 0;
    return f;
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
