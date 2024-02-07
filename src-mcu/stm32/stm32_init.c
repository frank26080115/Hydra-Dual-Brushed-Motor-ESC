#include "stm32.h"
#include "systick.h"

void SystemClock_Config(void);
void STM32_Sys_Init(void);

void mcu_init(void)
{
    SystemClock_Config(); // this starts the clocks

    #if defined(MCU_F051) || defined(MCU_F031)
    //LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_FLASH_EnablePrefetch();
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    #if defined(DEVELOPMENT_BOARD)
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    #endif
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1);
    //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
    //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);
    //LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM15);
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM16);
    //LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM17);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    //#ifdef LL_AHB1_GRP1_PERIPH_DMA2
    //LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
    //#endif
    #elif defined(MCU_G071)
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_FLASH_EnablePrefetch();
    //LL_SYSCFG_EnablePinRemap(LL_SYSCFG_PIN_RMP_PA11);
    //LL_SYSCFG_EnablePinRemap(LL_SYSCFG_PIN_RMP_PA12);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
    //LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
    //LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    //#if defined(DMA2)
    //LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
    //#endif
    #endif

    STM32_Sys_Init(); // this sets the vector table
    systick_init();
}
