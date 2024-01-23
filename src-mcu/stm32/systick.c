#include "systick.h"
#include "stm32.h"

volatile uint32_t systick_cnt;

void systick_init(void)
{
    systick_cnt = 0;
    SysTick_Config(SystemCoreClock / 1000); // starts systick at 1 ms intervals
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1); // low priority
    NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler(void)
{
    systick_cnt++;
}
