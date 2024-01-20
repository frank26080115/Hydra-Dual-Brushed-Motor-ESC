#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "conf.h"
#include "defs.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void led_task();
extern bool sense_task();

void delay_ms(uint32_t x);
int32_t fi_map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max, bool limit);
int fi_div_rounded(const int n, const int d);
int16_t rc_pulse_map(uint16_t x);
bool item_strcmp(const char* usr_inp, const char* table_item);
float pid_calc(pidloop_t* pidnow, int actual, int target);
uint8_t crsf_crc8(const uint8_t *ptr, int len);

#ifdef __cplusplus
}
#endif
