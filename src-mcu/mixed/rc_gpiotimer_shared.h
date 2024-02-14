// this file is meant to be included inline, do not include this file where it's not supposed to go

#define RC_MEASURE_MULTIPLIER 4 // speed up the timer to see if it can improve jitter

static
    #if defined(STMICRO)
        GPIO_TypeDef
    #elif defined(ARTERY)
        gpio_type
    #endif
        *rc_gpio;
static
    #if defined(STMICRO)
        TIM_TypeDef
    #elif defined(ARTERY)
        tmr_type
    #endif
        *rc_tim;

static uint32_t      rc_pin;
static uint32_t      rc_exti_line;
static uint32_t      rc_exti_sysline;
static uint32_t      rc_exti_port;

static volatile uint8_t  overflow_cnt;
static volatile uint16_t pulse_width;
static volatile bool     new_flag       = false;
static volatile uint32_t last_good_time = 0;
static volatile uint8_t  good_pulse_cnt = 0;
static volatile uint8_t  bad_pulse_cnt  = 0;
static volatile uint32_t arm_pulse_cnt  = 0;
static volatile bool     armed          = false;
static uint16_t arming_val_min = 0, arming_val_max = 0;

static volatile bool was_high;

#ifdef RC_LOG_JITTER
static volatile uint16_t pulse_width_prev = 0;
static volatile uint32_t jitter = 0;
#endif

RcPulse_GpioIsr::RcPulse_GpioIsr(void)
{
}

void RcPulse_GpioIsr::task(void)
{
    RcChannel::task();
}

int16_t RcPulse_GpioIsr::read(void)
{
    return rc_pulse_map((pulse_width + GPIO_RC_PULSE_OFFSET) / 4);
}

int16_t RcPulse_GpioIsr::readRaw(void)
{
    return pulse_width;
}

bool RcPulse_GpioIsr::is_alive(void)
{
    if ((millis() - last_good_time) < RC_INPUT_TIMEOUT)
    {
        if (good_pulse_cnt >= 3) {
            return true;
        }
    }
    new_flag = false;
    arm_pulse_cnt = 0;
    return false;
}

bool RcPulse_GpioIsr::has_new(bool clr)
{
    bool x = new_flag;
    if (clr) {
        new_flag = false;
    }
    return x;
}

bool RcPulse_GpioIsr::is_armed(void)
{
    if ((millis() - last_good_time) < disarm_timeout || disarm_timeout <= 0)
    {
        return armed;
    }
    armed = false;
    arm_pulse_cnt = 0;
    return false;
}

void RcPulse_GpioIsr::disarm(void)
{
    armed = false;
    arm_pulse_cnt = 0;
}

#ifdef RC_LOG_JITTER
RCPULSE_READJITTER(RcPulse_GpioIsr);
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(USE_LED_STRIP) && defined(MCU_AT421)
extern void WS2812_onIrq(void);
extern volatile bool WS2812_sendOccured;
#endif

void
#if defined(MCU_F051) || defined(MCU_GD32F350)
EXTI4_15_IRQHandler
#elif defined(MCU_G071)
EXTI4_15_IRQHandler
#elif defined(MCU_AT421)
EXINT15_4_IRQHandler
#else
#error
#endif
(void)
{
    uint32_t t = LL_TIM_GetCounter(rc_tim);
    #if defined(MCU_F051) || defined(MCU_GD32F350)
    LL_EXTI_ClearFlag_0_31(rc_exti_line);
    #endif
    #if defined(MCU_F051) || defined(MCU_AT421) || defined(MCU_GD32F350)
    if (LL_GPIO_IsInputPinSet(rc_gpio, rc_pin) == 0)
    #elif defined(MCU_G071)
    if (LL_EXTI_IsActiveFallingFlag_0_31(rc_exti_line))
    #else
    #error
    #endif
    {
        dbg_evntcnt_add(DBGEVNTID_GPIOTMR_FALL);
        #if defined(MCU_G071)
        LL_EXTI_ClearFallingFlag_0_31(rc_exti_line);
        #endif
        if (was_high
            #if defined(USE_LED_STRIP) && defined(MCU_AT421)
                && WS2812_sendOccured == false
            #endif
        )
        {
            if (overflow_cnt == 0 && t >= (RC_INPUT_VALID_MIN * RC_MEASURE_MULTIPLIER) && t <= (RC_INPUT_VALID_MAX * RC_MEASURE_MULTIPLIER)
                )
            {
                pulse_width = t;

                RCPULSE_LOGJITTER();

                rc_register_good_pulse(
                    pulse_width
                    , arming_val_min, arming_val_max
                    , (uint32_t*)&last_good_time
                    , (uint8_t*)&good_pulse_cnt, (uint8_t*)&bad_pulse_cnt, (uint32_t*)&arm_pulse_cnt
                    , (bool*)&new_flag, (bool*)&armed
                );
            }
            #if 0 // false pulses happen too frequently for arming to be effective, https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/2
            else
            {
                rc_register_bad_pulse((uint8_t*)&good_pulse_cnt, (uint8_t*)&bad_pulse_cnt, (uint32_t*)&arm_pulse_cnt);
            }
            #endif
        }
        was_high = false;
        #if defined(USE_LED_STRIP) && defined(MCU_AT421)
        // doing this here is safe because we have very guaranteed non-busy times
        WS2812_onIrq();
        WS2812_sendOccured = false; // does not need to invalidate the next pulse
        #endif
    }
    #if defined(MCU_F051) || defined(MCU_AT421) || defined(MCU_GD32F350)
    else
    #elif defined(MCU_G071)
    if (LL_EXTI_IsActiveRisingFlag_0_31(rc_exti_line))
    #else
    #error
    #endif
    {
        dbg_evntcnt_add(DBGEVNTID_GPIOTMR_RISE);
        #if defined(MCU_G071)
        LL_EXTI_ClearRisingFlag_0_31(rc_exti_line);
        #endif
        // reset counter on rising edge
        if (was_high == false)
        {
            LL_TIM_SetCounter(rc_tim, 0);
            overflow_cnt = 0;
        }
        was_high = true;
    }
}

// note: the overflow occurs every 16 milliseconds, which means it does occur at least once during one period of RC signaling
void
#if defined(MCU_F051)
TIM6_IRQHandler
#elif defined(MCU_GD32F350)
TIM6_DAC_IRQHandler
#elif defined(MCU_G071)
TIM6_DAC_LPTIM1_IRQHandler
#elif defined(MCU_AT421)
TMR6_GLOBAL_IRQHandler
#else
#error
#endif
(void)
{
    dbg_evntcnt_add(DBGEVNTID_GPIOTMR_OVERFLOW);
    if (LL_TIM_IsActiveFlag_UPDATE(rc_tim))
    {
        dbg_evntcnt_add(DBGEVNTID_GPIOTMR_OVERFLOW);
        LL_TIM_ClearFlag_UPDATE(rc_tim);
        if (overflow_cnt < 8) {
            overflow_cnt++;
            #if 0 // overflow happens too frequently for arming to be reliable, https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/2
            arm_pulse_cnt = 0;
            #endif
        }
        #if defined(USE_LED_STRIP) && defined(MCU_AT421)
        // doing this here is safe because we have very guaranteed non-busy times
        WS2812_onIrq();
        #endif
    }
}

#ifdef __cplusplus
}
#endif
