#include "systick.h"
#include "mcu.h"

volatile uint32_t systick_cnt;

#if defined(DISABLE_LED) || defined(ENABLE_TONE)
// stuff used for tone generation
extern volatile uint8_t tone_active;
extern volatile uint16_t tone_volume;
extern void pwm_set_all_duty_remapped(uint16_t a, uint16_t b, uint16_t c);
#endif

void systick_init(void)
{
    #if defined(DISABLE_LED) || defined(ENABLE_TONE)
    tone_freq_multi = 1;
    #endif
    systick_cnt = 0;
    SysTick_Config(SystemCoreClock / 1000); // starts systick at 1 ms intervals
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1); // low priority
    NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler(void)
{
    systick_cnt++;
    #if defined(DISABLE_LED) || defined(ENABLE_TONE)
    if (tone_active == 0) {
        return;
    }
    else if (tone_active == 1) {
        tone_active = 2;
        pwm_set_all_duty_remapped(tone_volume, 0, 0);
    }
    else if (tone_active == 2) {
        tone_active = 1;
        pwm_set_all_duty_remapped(0, tone_volume, tone_volume);
    }
    #endif
}
