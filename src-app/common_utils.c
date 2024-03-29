#include "funcs.h"
#include "main.h"
#include "systick.h"
#include "userconfig.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// the map function, but with rounded division and an option to limit the output
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

// implements division that's signed and rounded
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

// string comparison that ends at a space, also case-insensitive
bool item_strcmp(const char* usr_inp, const char* table_item, int* argi)
{
    int slen = 32; //strlen(usr_inp);
    int i;
    for (i = 0; i < slen; i++)
    {
        char inpc = usr_inp[i];
        char x = table_item[i];

        #if defined(MCU_G071)
        #define IS_CHAR_DELIMITER(ccc)    ((ccc) <= ' ' || (ccc) >= '~' || (ccc) == '=' || (ccc) == ':')
        #else
        #define IS_CHAR_DELIMITER(ccc)    ((ccc) <= ' ')
        #endif

        if (IS_CHAR_DELIMITER(inpc)) {
            inpc = 0;
        }
        if (IS_CHAR_DELIMITER(x)) {
            x = 0;
        }
        if (inpc == 0 && x == 0) {
            // both were delimited at the same time and so far has always matched
            if (argi != NULL) {
                *argi = i;
            }
            return true;
        }
        #if defined(MCU_G071)
        // case insensitive
        if (inpc >= 'A' && inpc <= 'Z') {
            inpc -= 'A';
            inpc += 'a';
        }
        if (x >= 'A' && x <= 'Z') {
            x -= 'A';
            x += 'a';
        }
        #endif
        if (inpc != x) {
            return false; // mismatch, immediate exit
        }
    }
    return false; // should never exit the loop because of string length
}

int32_t parse_integer(const char* str)
{
    int32_t v;
    #if defined(MCU_G071)
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        v = strtol(&str[2], NULL, 16);
    }
    else
    #endif
    {
        v = atoi(str);
    }
    return v;
}

int32_t pid_calc(pidloop_t* pidnow, int actual, int target)
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

uint8_t crsf_crc8(const uint8_t *ptr, int len)
{
    static const uint8_t crsf_crc8tab[256] = {
        0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D,
        0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F,
        0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9,
        0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B,
        0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0,
        0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2,
        0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44,
        0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16,
        0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92,
        0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0,
        0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36,
        0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64,
        0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F,
        0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D,
        0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB,
        0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9,
    };

    volatile uint8_t crc = 0;
    for (int i = 0; i < len; i++)
    {
        crc = crsf_crc8tab[crc ^ *ptr++];
    }
    return crc;
}

int32_t fi_lpf(int32_t oldval, int32_t newval, int16_t fltconst)
{
    if (fltconst <= 0 || fltconst >= 100) { // filter disabled
        return newval;
    }
    int32_t oldvalX = oldval * (100 - fltconst);
    int32_t newvalX = newval * fltconst;
    return (oldvalX + newvalX + 50) / 100; // division with rounding
}
