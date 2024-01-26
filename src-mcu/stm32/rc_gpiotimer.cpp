#include "rc.h"
#include "rc_stm32.h"
#include "gpio_to_exti.h"

#define RC_MEASURE_MULTIPLIER 4 // speed up the timer to see if it can improve jitter

static GPIO_TypeDef* rc_gpio;
static TIM_TypeDef*  rc_tim;
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

//#ifdef GPIOEXTI_IRQHandler

#ifdef __cplusplus
extern "C" {
#endif

void EXTI4_15_IRQHandler(void)
{
    uint32_t t = LL_TIM_GetCounter(rc_tim);
    #if defined(MCU_F051)
    LL_EXTI_ClearFlag_0_31(rc_exti_line);
    #endif
    #if defined(MCU_F051)
    if (LL_GPIO_IsInputPinSet(rc_gpio, rc_pin) == 0)
    #elif defined(MCU_G071)
    if (LL_EXTI_IsActiveFallingFlag_0_31(rc_exti_line))
    #endif
    {
        dbg_evntcnt_add(DBGEVNTID_GPIOTMR_FALL);
        #if defined(MCU_G071)
        LL_EXTI_ClearFallingFlag_0_31(rc_exti_line);
        #endif
        if (was_high)
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
            else
            {
                rc_register_bad_pulse((uint8_t*)&good_pulse_cnt, (uint8_t*)&bad_pulse_cnt, (uint32_t*)&arm_pulse_cnt);
            }
        }
        was_high = false;
    }
    #if defined(MCU_F051)
    else
    #elif defined(MCU_G071)
    if (LL_EXTI_IsActiveRisingFlag_0_31(rc_exti_line))
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
#elif defined(MCU_G071)
TIM6_DAC_LPTIM1_IRQHandler
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
            arm_pulse_cnt = 0;
        }
    }
}

#ifdef __cplusplus
}
#endif

//#endif

RcPulse_GpioIsr::RcPulse_GpioIsr(void)
{

}

void RcPulse_GpioIsr::init(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin)
{
    _tim  = TIMx;
    _gpio = GPIOx;
    _pin  = pin;

    rc_tim  = _tim;
    rc_gpio = _gpio;
    rc_pin  = _pin;

    rc_exti_line    = gpio_to_exti_line    (rc_pin);
    rc_exti_port    = gpio_to_exti_port    (rc_pin);
    rc_exti_sysline = gpio_to_exti_sys_line(rc_pin);

    LL_TIM_InitTypeDef timcfg = {0};
    timcfg.Prescaler     = __LL_TIM_CALC_PSC(SystemCoreClock, 1000000 * RC_MEASURE_MULTIPLIER);
    timcfg.CounterMode   = LL_TIM_COUNTERMODE_UP;
    timcfg.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    timcfg.Autoreload    = -1; // this will make it interrupt at overflow event
    LL_TIM_Init(rc_tim, &timcfg);
    // every count is 0.25 us , this works for both 8 MHz and 48 MHz system core clock

    LL_TIM_EnableIT_UPDATE(rc_tim);
    LL_TIM_EnableCounter(rc_tim);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin          = rc_pin;
    GPIO_InitStruct.Mode         = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed        = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType   = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull         = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(rc_gpio, &GPIO_InitStruct);

    #if defined(MCU_F051)
    LL_SYSCFG_SetEXTISource(rc_exti_port, rc_exti_sysline);
    #elif defined(MCU_G071)
    LL_EXTI_SetEXTISource(rc_exti_port, rc_exti_sysline);
    #else
    #error
    #endif
    LL_EXTI_EnableRisingTrig_0_31 (rc_exti_line);
    LL_EXTI_EnableFallingTrig_0_31(rc_exti_line);
    LL_EXTI_EnableEvent_0_31      (rc_exti_line);
    LL_EXTI_EnableIT_0_31         (rc_exti_line);

    NVIC_SetPriority(GPIOEXTI_TIM_IRQn, 1);
    NVIC_EnableIRQ  (GPIOEXTI_TIM_IRQn);
    NVIC_SetPriority(GPIOEXTI_IRQn, 0);
    NVIC_EnableIRQ  (GPIOEXTI_IRQn);

    was_high = LL_GPIO_IsInputPinSet(rc_gpio, rc_pin);

    rc_find_arming_vals(RC_MEASURE_MULTIPLIER, GPIO_RC_PULSE_OFFSET, (uint16_t*)&arming_val_min, (uint16_t*)&arming_val_max);
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
