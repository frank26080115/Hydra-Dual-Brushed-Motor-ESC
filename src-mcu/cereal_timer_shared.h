#define RC_IC_TIMx IC_TIMER_REGISTER

// only one instance of this cereal port is allowed
// so we use global static variables that can be access through ISRs

static uint32_t cereal_baud;
static fifo_t cereal_fifo_tx;
static fifo_t cereal_fifo_rx;
static volatile uint32_t last_rx_time = 0;
static volatile bool tx_is_busy = false;
static volatile bool rx_is_busy = false;
static volatile bool idle_flag_cleared = false;

extern void rc_ic_tim_init(void);
extern void rc_ic_tim_init_2(void);
extern bool ictimer_modeIsPulse;

Cereal_TimerBitbang::Cereal_TimerBitbang(void)
{
    _id = 0;
}

void Cereal_TimerBitbang::flush(void)
{
    while (
        //fifo_available(fifo_tx) &&
        tx_is_busy) {
        // do nothing but wait
    }
}

uint32_t Cereal_TimerBitbang::get_last_time(void)
{
    return last_rx_time;
}

bool Cereal_TimerBitbang::get_idle_flag(bool clr)
{
    // this implementation is never used, only data packet parsers use it
    // and those parsers always use USART, not bit-bang
    bool x = false;
    __disable_irq();
    x = (millis() - last_rx_time) > 100;
    if (x && idle_flag_cleared) {
        x = false;
    }
    if (clr) {
        idle_flag_cleared = true;
    }
    __enable_irq();
    return x;
}
