#include "stm32f0xx.h"

void NMI_Handler(void)
{
    NVIC_SystemReset();
}

void HardFault_Handler(void)
{
    NVIC_SystemReset();
    while (1) {
    }
}

void SVC_Handler(void)
{
    NVIC_SystemReset();
}

void PendSV_Handler(void)
{
    NVIC_SystemReset();
}
