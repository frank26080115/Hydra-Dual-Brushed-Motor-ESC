#include "rc.h"

uint32_t arm_pulses_required;
uint32_t disarm_timeout;

void RcChannel::init(void)
{

}

void RcChannel::task(void)
{

}

int16_t RcChannel::read(void)
{
    return 0;
}

bool RcChannel::is_alive(void)
{
    return false;
}

bool RcChannel::has_new(bool clr)
{
    return false;
}

bool is_armed(void)
{
    return false;
}

void disarm(void)
{

}