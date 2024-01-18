#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct fifo_t
{
    uint16_t size;
    uint8_t* buf;
    uint16_t pread;
    uint16_t pwrite;
};

static inline fifo_t* fifo_init(uint16_t sz)
{
    fifo_t* f = (fifo_t*)malloc(sizeof(fifo_t));
    f->buf = (uint8_t*)malloc((size_t)sz);
    f->size = sz;
    f->pread = 0;
    f->pwrite = 0;
}

static inline void fifo_reset(fifo_t* f)
{
    f->pread = 0;
    f->pwrite = 0;
}

static inline int16_t fifo_peek(fifo_t* f)
{
    if (f->pread == f->pwrite) {
        return -1;
    }
    return f->buf[f->pread];
}

static inline int16_t fifo_pop(fifo_t* f)
{
    if (f->pread == f->pwrite) {
        return -1;
    }
    uint8_t x = f->buf[f->pread];
    f->pread += 1;
    f->pread %= f->size;
    return (int16_t)x;
}

static inline void fifo_push(fifo_t* f, uint8_t x)
{
    uint16_t next = (f->pwrite + 1) % f->size;
    if (next != f->pread) {
        f->buf[f->pwrite] = x;
        f->pwrite = next;
    }
}

static inline int fifo_available(fifo_t* f) {
    uint32_t x = f->pwrite + f->size;
    x -= f->pread;
    x %= f->size;
    return x;
}

#ifdef __cplusplus
}
#endif
