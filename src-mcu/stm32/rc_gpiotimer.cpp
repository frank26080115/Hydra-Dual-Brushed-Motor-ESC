#include "rc.h"
#include "rc_stm32.h"

static GPIO_TypeDef* rc_gpio;
static TIM_TypeDef*  rc_tim;
static uint32_t      rc_pin;

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

#ifdef GPIOEXTI_IRQHandler

#ifdef __cplusplus
extern "C" {
#endif

void GPIOEXTI_IRQHandler(void)
{
    uint32_t t = LL_TIM_GetCounter(rc_tim);
    #if defined(MCU_F051)
    LL_EXTI_ClearFlag_0_31(GPIOEXTI_Line);
    #endif
    #if defined(MCU_F051)
    if (LL_GPIO_IsInputPinSet(rc_gpio, rc_pin) == 0)
    #elif defined(MCU_G071)
    if (LL_EXTI_IsActiveFallingFlag_0_31(GPIOEXTI_Line))
    #endif
    {
        #if defined(MCU_G071)
        LL_EXTI_ClearFallingFlag_0_31(GPIOEXTI_Line);
        #endif
        if (was_high)
        {
            if (overflow_cnt == 0 && t >= (RC_INPUT_VALID_MIN * 4) && t <= (RC_INPUT_VALID_MAX * 4))
            {
                pulse_width = t;
                last_good_time = millis();
                if (good_pulse_cnt < 64) {
                    good_pulse_cnt++;
                }
                bad_pulse_cnt = 0;
                new_flag = true;
                if (t >= arming_val_min && t <= arming_val_max) {
                    arm_pulse_cnt++;
                    if (arm_pulse_cnt >= arm_pulses_required) {
                        armed = true;
                    }
                }
                else {
                    arm_pulse_cnt = 0;
                }
            }
            else
            {
                if (bad_pulse_cnt < 3) {
                    bad_pulse_cnt++;
                }
                else {
                    good_pulse_cnt = 0;
                }
                arm_pulse_cnt = 0;
            }
        }
        was_high = false;
    }
    #if defined(MCU_F051)
    else
    #elif defined(MCU_G071)
    if (LL_EXTI_IsActiveRisingFlag_0_31(LL_EXTI_LINE_6))
    #endif
    {
        #if defined(MCU_G071)
        LL_EXTI_ClearRisingFlag_0_31(GPIOEXTI_Line);
        #endif
        // reset counter on rising edge
        LL_TIM_SetCounter(rc_tim, 0);
        overflow_cnt = 0;
        was_high = true;
    }
}

void GPIOEXTI_TIM_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(rc_tim))
    {
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

#endif

RcPulse_GpioIsr::RcPulse_GpioIsr(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin)
{
    _tim  = TIMx;
    _gpio = GPIOx;
    _pin  = pin;
}

void RcPulse_GpioIsr::init(void)
{
    rc_tim  = _tim;
    rc_gpio = _gpio;
    rc_pin  = _pin;

    LL_TIM_InitTypeDef timcfg = {0};
    timcfg.Prescaler     = __LL_TIM_CALC_PSC(SystemCoreClock, 4000000);
    timcfg.CounterMode   = LL_TIM_COUNTERMODE_UP;
    timcfg.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    timcfg.Autoreload    = 0; // this will make it interrupt at overflow event
    LL_TIM_Init(rc_tim, &timcfg);
    // every count is 0.25 us , this works for both 8 MHz and 48 MHz system core clock

    LL_TIM_EnableIT_UPDATE(rc_tim);
    NVIC_SetPriority(GPIOEXTI_TIM_IRQn, 0);
    NVIC_EnableIRQ(GPIOEXTI_TIM_IRQn);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin          = rc_pin;
    GPIO_InitStruct.Mode         = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed        = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType   = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull         = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(rc_gpio, &GPIO_InitStruct);

    #if defined(MCU_F051)
    LL_SYSCFG_SetEXTISource(GPIOEXTI_Port, GPIOEXTI_SYSCFG_Line);
    #elif defined(MCU_G071)
    LL_EXTI_SetEXTISource(GPIOEXTI_Port, GPIOEXTI_SYSCFG_Line);
    #endif
    LL_EXTI_EnableRisingTrig_0_31(GPIOEXTI_Line);
    LL_EXTI_EnableFallingTrig_0_31(GPIOEXTI_Line);
    LL_EXTI_EnableEvent_0_31(GPIOEXTI_Line);
    LL_EXTI_EnableIT_0_31(GPIOEXTI_Line);

    NVIC_SetPriority(GPIOEXTI_IRQn, 0);
    NVIC_EnableIRQ(GPIOEXTI_IRQn);

    was_high = LL_GPIO_IsInputPinSet(rc_gpio, rc_pin);

    uint16_t test_arming_val = 0;
    int last_v = -THROTTLE_UNIT_RANGE;
    for (test_arming_val = 1250 * 4; test_arming_val < 1750 * 4; test_arming_val++) {
        int v = rc_pulse_map((test_arming_val + GPIO_RC_PULSE_OFFSET) / 4);
        if (v == 0 && last_v < 0 && arming_val_min == 0) {
            arming_val_min = test_arming_val - 1;
        }
        if (v == 0 && last_v > 0 && arming_val_max == 0) {
            arming_val_max = test_arming_val;
            break;
        }
        last_v = v;
    }
}

void RcPulse_GpioIsr::task(void)
{
    RcChannel::task();
}

int16_t RcPulse_GpioIsr::read(void)
{
    return rc_pulse_map((pulse_width + GPIO_RC_PULSE_OFFSET) / 4);
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
