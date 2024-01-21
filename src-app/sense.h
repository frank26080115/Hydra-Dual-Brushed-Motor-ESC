#pragma once

#include "main.h"
#include "mcu.h"

#ifdef __cplusplus
extern "C" {
#endif

void sense_init(void);
void adc_init(void);

bool sense_task(void);
bool adc_task(void);

extern uint32_t sense_current;      // TODO calibrate
extern uint32_t sense_voltage;      // millivolts
extern uint32_t sense_temperatureC; // C

extern uint16_t adc_raw_voltage;
extern uint16_t adc_raw_current;
extern uint16_t adc_raw_temperature;

extern bool sense_newData;

extern pidloop_t current_pid;
extern int16_t current_limit_val;
void current_limit_task(void);

#ifdef __cplusplus
}
#endif
