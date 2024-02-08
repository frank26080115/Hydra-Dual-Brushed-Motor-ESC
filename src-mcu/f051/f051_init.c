#include "stm32.h"

void SystemClock_Config(void)
{
    #ifndef AGGRESSIVE_BOOT // disable this section to save code space, as there is a bootloader that has already done all this
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

    if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_1) {
        // Error_Handler();
    }
    LL_RCC_HSI_Enable();

    /* Wait till HSI is ready */
    while (LL_RCC_HSI_IsReady() == 0) {
    }
    LL_RCC_HSI_SetCalibTrimming(16);

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() == 0) {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
    }
    #endif

    LL_Init1msTick(48000000);
    LL_SetSystemCoreClock(48000000);

    LL_RCC_HSI14_Enable();
    /* Wait till HSI14 is ready */
    while (LL_RCC_HSI14_IsReady() == 0) {
    }
    LL_RCC_HSI14_SetCalibTrimming(16);
    LL_RCC_LSI_Enable();
    /* Wait till LSI is ready */
    while (LL_RCC_LSI_IsReady() == 0) {
    }
}

void STM32_Sys_Init(void)
{
    volatile uint32_t* VectorTable = (volatile uint32_t*)0x20000000;
    uint32_t vector_index = 0;
    for (vector_index  = 0; vector_index  < 48; vector_index++) {
        VectorTable[vector_index ] = *(__IO uint32_t*)((
            APPLICATION_ADDRESS
            #ifdef DEVELOPMENT_BOARD
                & 0xFF000000
            #endif
            ) + (vector_index << 2));        // no VTOR on cortex-MO so need to copy vector table
    }

    /* Enable the SYSCFG peripheral clock*/
    do {
        volatile uint32_t tmpreg;
        ((((RCC_TypeDef*)((((uint32_t)0x40000000U) + 0x00020000) + 0x00001000))->APB2ENR) |= ((0x1U << (0U))));
        /* Delay after an RCC peripheral clock enabling */
        tmpreg = ((((RCC_TypeDef*)((((uint32_t)0x40000000U) + 0x00020000) + 0x00001000))->APB2ENR) & ((0x1U << (0U))));
        ((void)(tmpreg));
    }
    while (0U);
    //      /* Remap SRAM at 0x00000000 */
    do {
        ((SYSCFG_TypeDef*)(((uint32_t)0x40000000U) + 0x00010000))->CFGR1 &= ~((0x3U << (0U)));
        ((SYSCFG_TypeDef*)(((uint32_t)0x40000000U) + 0x00010000))->CFGR1 |= ((0x1U << (0U)) | (0x2U << (0U)));
    }
    while (0);
    __enable_irq();

    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
}
