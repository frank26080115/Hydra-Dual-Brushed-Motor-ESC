// this file is meant to be included inline, do not include this file where it's not supposed to go

#define RC_IC_TIMx IC_TIMER_REGISTER

static volatile uint16_t pulse_width;
static volatile bool     new_flag       = false;
static volatile uint32_t last_good_time = 0;
static volatile uint8_t  good_pulse_cnt = 0;
static volatile uint8_t  bad_pulse_cnt  = 0;
static volatile uint32_t arm_pulse_cnt  = 0;
static volatile bool     armed          = false;
static uint16_t arming_val_min = 0, arming_val_max = 0;

#ifdef RC_LOG_JITTER
static volatile uint16_t pulse_width_prev = 0;
static volatile uint32_t jitter = 0;
#endif

#ifdef ENABLE_COMPILE_CLI
bool ictimer_modeIsPulse;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_COMPILE_CLI
extern void CerealBitbang_IRQHandler(void);
#endif

#ifdef __cplusplus
}
#endif

void RcPulse_InputCap::task(void)
{
    RcChannel::task();
    uint32_t now = millis();
    if (last_good_time > now && now < 0x7FFFFFFF && armed) {
        dbg_printf("RcPulse_InputCap unexpected future rx time %u > %u\r\n", last_good_time, now);
        armed = false;
        arm_pulse_cnt = 0;
    }
}

int16_t RcPulse_InputCap::read(void)
{
    return rc_pulse_map(pulse_width);
}

int16_t RcPulse_InputCap::readRaw(void)
{
    return pulse_width;
}

bool RcPulse_InputCap::is_alive(void)
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

bool RcPulse_InputCap::has_new(bool clr)
{
    bool x = new_flag;
    if (clr) {
        new_flag = false;
    }
    return x;
}

bool RcPulse_InputCap::is_armed(void)
{
    if ((millis() - last_good_time) < disarm_timeout || disarm_timeout <= 0)
    {
        return armed;
    }
    armed = false;
    arm_pulse_cnt = 0;
    return false;
}

void RcPulse_InputCap::disarm(void)
{
    armed = false;
    arm_pulse_cnt = 0;
}

#ifdef RC_LOG_JITTER
RCPULSE_READJITTER(RcPulse_InputCap);
#endif

#ifdef __cplusplus
extern "C" {
#endif

void
#if defined(MAIN_SIGNAL_PB4) || defined(MAIN_SIGNAL_PA6)
#if defined(STMICRO)
TIM3_IRQHandler
#elif defined(ARTERY)
TMR3_GLOBAL_IRQHandler
#else
#error
#endif
#elif defined(MAIN_SIGNAL_PA2)
#if defined(STMICRO)
TIM15_IRQHandler
#elif defined(ARTERY)
TMR15_GLOBAL_IRQHandler
#else
#error
#endif
#else
#error
#endif
(void)
{
    #ifdef ENABLE_COMPILE_CLI
    if (ictimer_modeIsPulse)
    #endif
    {
        dbg_evntcnt_add(DBGEVNTID_ICTIMER);

        uint32_t p = RC_IC_TIMx->CCR1;   // Pulse period
        uint32_t w = RC_IC_TIMx->CCR2;   // Pulse width

        // reading the registers should automatically clear the interrupt flags

        if (p < RC_INPUT_VALID_MAX || w < RC_INPUT_VALID_MIN || w > RC_INPUT_VALID_MAX) // out of range
        {
            rc_register_bad_pulse((uint8_t*)&good_pulse_cnt, (uint8_t*)&bad_pulse_cnt, (uint32_t*)&arm_pulse_cnt);
        }
        else
        {
            pulse_width = w;

            RCPULSE_LOGJITTER();

            rc_register_good_pulse(
                pulse_width
                , arming_val_min, arming_val_max
                , (uint32_t*)&last_good_time
                , (uint8_t*)&good_pulse_cnt, (uint8_t*)&bad_pulse_cnt, (uint32_t*)&arm_pulse_cnt
                , (bool*)&new_flag, (bool*)&armed
            );
        }
    }
    #ifdef ENABLE_COMPILE_CLI
    else
    {
        CerealBitbang_IRQHandler();
    }
    #endif
}

#ifdef __cplusplus
}
#endif
