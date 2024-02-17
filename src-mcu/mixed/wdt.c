#include "wdt.h"

#ifndef LSI_VALUE
#define LSI_VALUE 32000
#endif

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
    #if defined(STMICRO)
    LL_IWDG_ReloadCounter(IWDG);
    #elif defined(ARTERY)
    wdt_counter_reload();
    #endif
}
