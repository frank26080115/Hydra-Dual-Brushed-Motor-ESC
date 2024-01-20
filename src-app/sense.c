#include "sense.h"
#include "types.h"

extern EEPROM_data_t cfg;

float sense_current = 0;
float sense_voltage = 0;
float sense_temperatureC = 0;
bool  sense_newData = false;
uint16_t adc_raw_voltage = 0;
uint16_t adc_raw_current = 0;
uint16_t adc_raw_temperature = 0;

float current_filter = 0;

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

        float filter_const = cfg.adc_filter;
        filter_const /= 1000.0f;

        float tempC = __LL_ADC_CALC_TEMPERATURE(3300, adc_raw_temperature, LL_ADC_RESOLUTION_12B);
        if (sense_temperatureC == 0) {
            sense_temperatureC = tempC;
        }
        sense_temperatureC = (sense_temperatureC * (1.0f - filter_const)) + (tempC * filter_const);

        float millivolts = (adc_raw_voltage * 3300 / 4095 * cfg.voltage_divider);
        if (sense_voltage == 0) {
            sense_voltage = millivolts / 1000.0f;
        }
        sense_voltage = ((millivolts / 1000.0f) * (1.0f - filter_const)) + (sense_voltage * filter_const);

        float fcurrent = adc_raw_current;
        current_filter = (current_filter * (1.0f - filter_const)) + (fcurrent * filter_const);
        sense_current = ((current_filter * 3300 / 41) - (cfg.current_offset * 100)) / (cfg.current_scale);

        return true;
    }
    return false;
}
