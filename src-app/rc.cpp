#include "rc.h"

uint32_t arm_pulses_required;
uint32_t disarm_timeout;

// dummy empty functions so that the linker does not complain

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

int16_t RcChannel::readRaw(void)
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

bool RcChannel::is_armed(void)
{
    return false;
}

void RcChannel::disarm(void)
{

}

#ifdef RC_LOG_JITTER
uint32_t RcChannel::readJitter(void)
{
    return 0;
}
#endif

bool rc_find_arming_vals(int32_t multi, int32_t offset, uint16_t* out_min, uint16_t* out_max)
{
    if (eeprom_has_loaded == false) {
        (*out_min) = 1480 * multi;
        (*out_max) = 1520 * multi;
        return false;
    }
    uint16_t test_arming_val;
    int last_v = -THROTTLE_UNIT_RANGE;
    for (test_arming_val = 1250 * multi; test_arming_val < 1750 * multi; test_arming_val++) {
        int v = rc_pulse_map((test_arming_val + offset) / multi);
        if (v == 0 && last_v < 0 && (*out_min) == 0) {
            (*out_min) = test_arming_val - 1;
        }
        if (v == 0 && last_v > 0 && (*out_max) == 0) {
            (*out_max) = test_arming_val;
            return true;
        }
        last_v = v;
    }
    return false;
}

void rc_register_good_pulse(uint16_t pw, uint16_t arm_thresh_min, uint16_t arm_thresh_max, uint32_t* timestamp, uint8_t* good_cnt, uint8_t* bad_cnt, uint32_t* arm_cnt, bool* nflag, bool* armed)
{
    (*timestamp) = millis();
    if ((*good_cnt) < 128) {
        (*good_cnt)++;
    }
    (*bad_cnt) = 0;
    (*nflag) = true;
    if (armed != NULL && arm_cnt != NULL)
    {
        if (arm_pulses_required > 0)
        {
            if (pw >= arm_thresh_min && pw <= arm_thresh_max) {
                (*arm_cnt)++;
                if ((*arm_cnt) >= arm_pulses_required) {
                    (*armed) = true;
                }
            }
            else {
                (*arm_cnt) = 0;
            }
        }
        else {
            (*armed) = true;
        }
    }
}

void rc_register_bad_pulse(uint8_t* good_cnt, uint8_t* bad_cnt, uint32_t* arm_cnt)
{
    if ((*bad_cnt) < 4) {
        (*bad_cnt)++;
    }
    else {
        (*good_cnt) = 0;
    }
    if (arm_cnt != NULL) {
        (*arm_cnt) = 0;
    }
}
