#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "conf.h"
#include "defs.h"
#include "types.h"

#include "mcu.h"
#include "funcs.h"

#define DEBUG_BAUD 115200

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_PRINT
void dbg_printf(const char* fmt, ...);
#else
#define dbg_printf(...)
//#define dbg_button_init(...)
//#define dbg_read_btn(...)      false
#endif

void dbg_button_init(void);
bool dbg_read_btn(void);

enum
{
    DBGEVNTID_ADC                = 0,
    DBGEVNTID_BITBANG            = 1,
    DBGEVNTID_USART_RX           = 2,
    DBGEVNTID_USART_TX           = 3,
    DBGEVNTID_USART_IDLE         = 4,
    DBGEVNTID_GPIOTMR_RISE       = 5,
    DBGEVNTID_GPIOTMR_FALL       = 6,
    DBGEVNTID_GPIOTMR_OVERFLOW   = 7,
    DBGEVNTID_ICTIMER            = 8,
};

#ifdef DEBUG_EVENTCNT
void dbg_evntcnt_add(uint8_t id);
uint32_t dbg_evntcnt_get(uint8_t id);
#else
#define dbg_evntcnt_add(...)
#define dbg_evntcnt_get(...)
#endif

#ifdef DEBUG_PINTOGGLE
void dbg_pintoggle_init(void);
void dbg_pinset(uint8_t p, bool high);
void dbg_pintoggle(uint8_t p);
#else
#define dbg_pintoggle_init(...)
#define dbg_pinset(...)
#define dbg_pintoggle(...)
#endif

#if defined(STM32F051DISCO)
void dbg_switch_to_pwm(void);
void dbg_switch_to_cereal(void);
#else
#define dbg_switch_to_pwm(...)
#define dbg_switch_to_cereal(...)
#endif

#ifdef RC_LOG_JITTER
#define RCPULSE_LOGJITTER()        do {               \
    uint32_t diff = (pulse_width_prev != 0) ? ((pulse_width >= pulse_width_prev) ? (pulse_width - pulse_width_prev) : (pulse_width_prev - pulse_width)) : 0; \
    jitter = fi_lpf(jitter, diff * 1000, 100);        \
    pulse_width_prev = pulse_width;                   \
} while (0)                                           \


#define RCPULSE_READJITTER(cn)    uint32_t cn::readJitter(void) { return (jitter + 500) / 1000; }
#else
#define RCPULSE_LOGJITTER(...)
#endif

#ifdef __cplusplus
}
#endif

extern void dbg_wait_user(void);
