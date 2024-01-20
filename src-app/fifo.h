#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mcu.h"

#define FIFO_DISABLE_IRQ()    do { if (is_in_isr() == 0) { __disable_irq(); } } while (0)
#define FIFO_ENABLE_IRQ()     do { if (is_in_isr() == 0) { __enable_irq();  } } while (0)

typedef struct
{
    uint16_t size;
    uint8_t* buf;
    uint16_t pread;
    uint16_t pwrite;
}
fifo_t;

static inline fifo_t* fifo_init(uint16_t sz)
{
    fifo_t* f = (fifo_t*)malloc(sizeof(fifo_t));
    f->buf = (uint8_t*)malloc((size_t)sz);
    f->size = sz;
    f->pread = 0;
    f->pwrite = 0;
    return f;
}

static inline void fifo_reset(fifo_t* f)
{
    FIFO_DISABLE_IRQ();
    f->pread = 0;
    f->pwrite = 0;
    FIFO_ENABLE_IRQ();
}

static inline int16_t fifo_peek(fifo_t* f)
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

static inline int16_t fifo_peektail(fifo_t* f)
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

static inline int16_t fifo_pop(fifo_t* f)
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

static inline void fifo_push(fifo_t* f, uint8_t x)
{
    FIFO_DISABLE_IRQ();
    uint16_t next = (f->pwrite + 1) % f->size;
    if (next != f->pread) {
        f->buf[f->pwrite] = x;
        f->pwrite = next;
    }
    FIFO_ENABLE_IRQ();
}

static inline int fifo_available(fifo_t* f) {
    FIFO_DISABLE_IRQ();
    uint32_t x = f->pwrite + f->size;
    x -= f->pread;
    x %= f->size;
    FIFO_ENABLE_IRQ();
    return x;
}

#ifdef __cplusplus
}
#endif
