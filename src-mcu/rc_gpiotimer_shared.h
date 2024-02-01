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
