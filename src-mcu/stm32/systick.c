#include "systick.h"
#include "stm32.h"

void systick_init(void)
{
    #if 0 // systick is already initialized in SystemClock_Config
    SysTick_Config(SystemCoreClock / 1000); // starts systick at 1 ms intervals
    #endif
}
