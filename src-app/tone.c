#include "tone.h"
#include "systick.h"
#include "phaseout.h"

#if defined(DISABLE_LED) || defined(ENABLE_TONE)

static uint32_t start_time = 0;
static uint32_t tone_duration = 0;
volatile uint8_t tone_active;
volatile uint32_t tone_freq_multi;
volatile uint16_t tone_volume;

extern void load_runtime_configs(void);

void tone_task(void)
{
    if (tone_active != 0)
    {
        if (tone_duration > 0)
        {
            if ((millis() - start_time) >= tone_duration)
            {
                tone_stop();
            }
        }
    }
}

void tone_start(uint8_t freq_multi, uint32_t duration, uint8_t volume)
{
    __disable_irq();
    start_time = millis();

    if (volume > 100) {
        volume = TONE_DEF_VOLUME;
    }

    tone_duration = duration;
    tone_freq_multi = freq_multi <= 0 ? 1 : freq_multi;
    tone_volume = fi_map(volume, 0, 100, 0, PWM_TONE_RELOAD - PWM_DEFAULT_HEADROOM, true);
    SysTick_Config(SystemCoreClock / (1000 * freq_multi));
    tone_active = tone_volume > 0 ? 1 : 0;
    pwm_set_all_duty_remapped(0, 0, 0);
    if (tone_active == 0) {
        load_runtime_configs();
    }
    else {
        pwm_set_reload(PWM_TONE_RELOAD);
    }
    systick_cnt = start_time * tone_freq_multi; // make sure the rest of the application still has a normal system time
    __enable_irq();
}

void tone_stop(void)
{
    if (tone_active == 0) {
        return;
    }
    tone_start(1, 0, 0); // stops because volume is 0, the freq multiplier of 1 restores systick
}

bool tone_isBusy(void)
{
    return tone_active != 0;
}

#endif
