#include "sense.h"
#include "types.h"
#include "funcs.h"
#include "systick.h"

extern EEPROM_data_t cfg;

uint32_t sense_current = 0;
uint32_t sense_voltage = 0;
uint32_t sense_temperatureC = 0;
bool  sense_newData = false;
uint16_t adc_raw_voltage = 0;
uint16_t adc_raw_current = 0;
uint16_t adc_raw_temperature = 0;
uint16_t adc_raw_current_filtered = 0;

pidloop_t current_pid = {
    .Kp = 400,               // can be modified later through user configuration
    .Ki = 0,                 // can be modified later through user configuration
    .Kd = 1000,              // can be modified later through user configuration
    .integral_limit = 20000, // cannot be modified later
    .output_limit = 100000   // cannot be modified later
    // values are large, but the output will be divided later
};
int16_t current_limit_val = 0;

void sense_init(void)
{
    adc_init();
}

bool sense_task(void)
{
    bool new_data = adc_task();
    if (new_data)
    {
        sense_newData = true;

        uint16_t filter_const = cfg.adc_filter;

        uint16_t tempC = __LL_ADC_CALC_TEMPERATURE(3300, adc_raw_temperature, LL_ADC_RESOLUTION_12B);
        if (sense_temperatureC == 0) {
            sense_temperatureC = tempC;
        }
        sense_temperatureC = fi_lpf(sense_temperatureC, tempC, filter_const);

        uint16_t millivolts = (adc_raw_voltage * 3300 / 4095 * cfg.voltage_divider);
        if (sense_voltage == 0) {
            sense_voltage = millivolts;
        }
        sense_voltage = fi_lpf(sense_voltage, millivolts, filter_const);

        adc_raw_current_filtered = fi_lpf(adc_raw_current_filtered, adc_raw_current, filter_const);
        sense_current = ((adc_raw_current_filtered * 3300 / 41) - (cfg.current_offset * 100)) / (cfg.current_scale);

        return true;
    }
    return false;
}

void current_limit_task(void)
{
    static uint32_t last_time = 0;
    if (cfg.current_limit <= 0) {
        current_limit_val = 0;
        return;
    }
    // calculations needs to happen at 1 kHz intervals
    uint32_t now = millis();
    if (last_time == now) {
        return;
    }
    last_time = now;
    current_limit_val = pid_calc(&current_pid, sense_current, cfg.current_limit * 100) / 10000;
}
