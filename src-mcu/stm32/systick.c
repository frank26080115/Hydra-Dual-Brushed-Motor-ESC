#include "systick.h"
#include "stm32.h"

void systick_init(void)
{
    SysTick_Config(SystemCoreClock / 1000); // starts systick at 1 ms intervals
}
