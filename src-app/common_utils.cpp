#include "funcs.h"
#include "main.h"
#include "userconfig.h"

#include <math.h>

void delay_ms(uint32_t x)
{
    uint32_t t = millis();
    while ((millis() - t) < x) {
        led_task();
        sense_task();
    }
}

int32_t fi_map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max, bool limit)
{
    int32_t a = x - in_min;
    int32_t b = out_max - out_min;
    int32_t c = in_max - in_min;
    int32_t d = a * b;
    int32_t y = fi_div_rounded(d, c);
    y += out_min;
    if (limit)
    {
        if (out_max > out_min)
        {
            if (y > out_max) {
                return out_max;
            }
            else if (y < out_min) {
                return out_min;
            }
        }
        else
        {
            if (y < out_max) {
                return out_max;
            }
            else if (y > out_min) {
                return out_min;
            }
        }
    }
    return y;
}

int fi_div_rounded(const int n, const int d)
{
    return ((n < 0) ^ (d < 0)) ? ((n - (d / 2)) / d) : ((n + (d / 2)) / d);
}

int16_t rc_pulse_map(uint16_t x)
{
    int32_t x2 = x;
    x2 -= cfg.rc_mid;
    x2 = x2 >= cfg.rc_deadzone ? (x2 - cfg.rc_deadzone) : (-x2 >= cfg.rc_deadzone ? (x2 + cfg.rc_deadzone) : (0));
    if (x2 == 0) {
        return 0;
    }
    int32_t nrange = cfg.rc_range - cfg.rc_deadzone;
    return fi_map(x2, -nrange, nrange, -THROTTLE_UNIT_RANGE, THROTTLE_UNIT_RANGE, true);
}

bool item_strcmp(const char* usr_inp, const char* table_item)
{
    int slen = strlen(usr_inp);
    int i;
    for (i = 0; i < slen; i++)
    {
        char inpc = usr_inp[i];
        char x = table_item[i];
        if (inpc <= ' ' || inpc >= '~') {
            inpc = 0;
        }
        if (x <= ' ' || x >= '~') {
            x = 0;
        }
        if (inpc == 0 && x == 0) {
            return true;
        }
        if (inpc >= 'A' && inpc <= 'Z') {
            inpc -= 'A';
            inpc += 'a';
        }
        if (x >= 'A' && x <= 'Z') {
            x -= 'A';
            x += 'a';
        }
        if (inpc != x) {
            return false;
        }
    }
    return true;
}

float pid_calc(pidloop_t* pidnow, int actual, int target)
{
    // this is expected to run at 1 kHz loop time

    pidnow->error = actual - target;
    pidnow->integral = pidnow->integral + pidnow->error * pidnow->Ki;
    if (pidnow->integral > pidnow->integral_limit) {
        pidnow->integral = pidnow->integral_limit;
    }
    if (pidnow->integral < -pidnow->integral_limit) {
        pidnow->integral = -pidnow->integral_limit;
    }

    pidnow->derivative = pidnow->Kd * (pidnow->error - pidnow->last_error);
    pidnow->last_error = pidnow->error;

    pidnow->pid_output = pidnow->error * pidnow->Kp + pidnow->integral + pidnow->derivative;

    if (pidnow->pid_output > pidnow->output_limit) {
        pidnow->pid_output = pidnow->output_limit;
    }
    if (pidnow->pid_output < -pidnow->output_limit) {
        pidnow->pid_output = -pidnow->output_limit;
    }
    return pidnow->pid_output;
}
