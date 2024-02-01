#include "rc.h"
#include "rc_at32.h"
#include "gpio_to_exti.h"

#define RC_MEASURE_MULTIPLIER 4 // speed up the timer to see if it can improve jitter

static gpio_type*    rc_gpio;
static tmr_type*     rc_tim;
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

#ifdef __cplusplus
extern "C" {
#endif

void EXINT15_4_IRQHandler(void)
{
    uint32_t t = tmr_counter_value_get(rc_tim);
    exint_flag_clear(rc_exti_line);
    if (gpio_input_data_bit_read(rc_gpio, rc_pin) == 0)
    {
        dbg_evntcnt_add(DBGEVNTID_GPIOTMR_FALL);
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
        #ifdef USE_LED_STRIP
        WS2812_onIrq();
        #endif
    }
    else
    {
        dbg_evntcnt_add(DBGEVNTID_GPIOTMR_RISE);
        // reset counter on rising edge
        if (was_high == false)
        {
            tmr_counter_value_set(rc_tim, 0);
            overflow_cnt = 0;
        }
        was_high = true;
    }
}

#ifdef USE_LED_STRIP
extern WS2812_onIrq(void);
#endif

// note: the overflow occurs every 16 milliseconds, which means it does occur at least once during one period of RC signaling
void TIM6_GLOBAL_IRQHandler(void)
{
    dbg_evntcnt_add(DBGEVNTID_GPIOTMR_OVERFLOW);
    if (tmr_flag_get(rc_tim, TMR_OVF_FLAG))
    {
        dbg_evntcnt_add(DBGEVNTID_GPIOTMR_OVERFLOW);
        tmr_flag_clear(rc_tim, TMR_OVF_FLAG);
        if (overflow_cnt < 8) {
            overflow_cnt++;
            arm_pulse_cnt = 0;
        }
        #ifdef USE_LED_STRIP
        WS2812_onIrq();
        #endif
    }
}

#ifdef __cplusplus
}
#endif

RcPulse_GpioIsr::RcPulse_GpioIsr(void)
{

}

void RcPulse_GpioIsr::init(tmr_type* TIMx, gpio_type* GPIOx, uint32_t pin)
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

    exint_init_type cfg;
    exint_default_para_init(&cfg);
    cfg.line_mode     = EXINT_LINE_INTERRUPUT;
    cfg.line_select   = rc_exti_line;
    cfg.line_polarity = EXINT_TRIGGER_BOTH_EDGE;
    cfg.line_enable   = 0;
    exint_init(&cfg);
    scfg_exint_line_config(rc_exti_port, rc_exti_sysline);

    tmr_clock_source_div_set(rc_tim, TMR_CLOCK_DIV1);
    rc_tim->div = (SystemCoreClock / (1000000 * RC_MEASURE_MULTIPLIER)) - 1; // 0.25us resolution
    rc_tim->pr = -1;

    tmr_interrupt_enable(rc_tim, TMR_OVF_INT, true);
    tmr_counter_enable(rc_tim);

    gpio_init_type GPIO_InitStruct = { 0 };
    GPIO_InitStruct.gpio_mode           = GPIO_MODE_INPUT;
    GPIO_InitStruct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    GPIO_InitStruct.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
    GPIO_InitStruct.gpio_pull           = GPIO_PULL_DOWN;
    GPIO_InitStruct.gpio_pins           = rc_pin;
    gpio_init(rc_gpio, &GPIO_InitStruct);

    NVIC_SetPriority(TMR6_GLOBAL_IRQn, 1);
    NVIC_EnableIRQ  (TMR6_GLOBAL_IRQn);
    NVIC_SetPriority(EXINT15_4_IRQn, 0);
    NVIC_EnableIRQ  (EXINT15_4_IRQn);

    was_high = gpio_input_data_bit_read(rc_gpio, rc_pin);

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
