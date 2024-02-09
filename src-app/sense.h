#pragma once

#include "main.h"
#include "mcu.h"

#ifdef __cplusplus
extern "C" {
#endif

void sense_init(void);
extern void adc_init(void);

bool sense_task(void);
extern bool adc_task(void);

extern uint32_t sense_current;      // TODO calibrate
extern uint32_t sense_voltage;      // millivolts
extern uint32_t sense_temperatureC; // C
extern uint32_t battery_voltage;

extern uint16_t adc_raw_voltage;
extern uint16_t adc_raw_current;
extern uint16_t adc_raw_temperature;

extern bool sense_newData;

extern pidloop_t current_pid;
extern uint16_t current_limit_duty;
void current_limit_task(void);

extern uint32_t voltage_limit;
void battery_task(void);
void battery_reset(void);

void load_config_pid(void);

#ifdef __cplusplus
}
#endif
