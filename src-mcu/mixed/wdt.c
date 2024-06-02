#include "wdt.h"

#ifndef LSI_VALUE
#define LSI_VALUE 32000
#endif

static volatile bool    has_armed = false; // only require two bits set if an initial connection has been established
static volatile uint8_t req_flags = 0;     // two bit flags must be set for watchdog to be fed

void wdt_init(void)
{
    #define IWDG_TICKS    (1 * (LSI_VALUE/16)) // one second, 32 kHz LSI, 16 divider
    #if defined(STMICRO)
    LL_IWDG_EnableWriteAccess(IWDG);
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_16);
    LL_IWDG_SetReloadCounter(IWDG, IWDG_TICKS);
    LL_IWDG_Enable(IWDG);
    #elif defined(ARTERY)
    wdt_register_write_enable(TRUE);
    wdt_divider_set(WDT_CLK_DIV_16);
    wdt_reload_value_set(IWDG_TICKS);
    wdt_enable();
    #else
    #error
    #endif
}

void wdt_feed(void)
{
    if (req_flags != 0x03 && has_armed) {
        return;
    }
    #if defined(STMICRO)
    LL_IWDG_ReloadCounter(IWDG);
    #elif defined(ARTERY)
    wdt_counter_reload();
    #endif
    req_flags = 0;
}

void wdt_feed_fromMain(bool armed)
{
    if (armed) {
        has_armed = true;
    }
    req_flags |= 0x01;
    wdt_feed();
}

void wdt_feed_onPulse(void)
{
    req_flags |= 0x02;
    wdt_feed();
}
