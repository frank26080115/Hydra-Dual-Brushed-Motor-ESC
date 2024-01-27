#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
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
void    fifo_init(fifo_t* f, void* buff, uint16_t sz);
void    fifo_reset(fifo_t* f);
int16_t fifo_peek(fifo_t* f);
int16_t fifo_peekAt(fifo_t* f, int16_t i);
int16_t fifo_peekTail(fifo_t* f);
int16_t fifo_pop(fifo_t* f);
void    fifo_push(fifo_t* f, uint8_t x);
int     fifo_available(fifo_t* f);
int16_t fifo_consume(fifo_t* f, uint16_t x);
int     fifo_readN(fifo_t* f, uint8_t* dst, uint16_t len);
bool    fifo_popUntil(fifo_t* f, uint8_t x);

#ifdef __cplusplus
}
#endif
