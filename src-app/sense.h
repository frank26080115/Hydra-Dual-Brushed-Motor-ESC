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

extern float sense_current;
extern float sense_voltage;
extern float sense_temperatureC;

extern uint16_t adc_raw_voltage;
extern uint16_t adc_raw_current;
extern uint16_t adc_raw_temperature;

extern bool sense_newData;

#ifdef __cplusplus
}
#endif
