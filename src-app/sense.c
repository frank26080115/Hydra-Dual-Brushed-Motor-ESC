#include "sense.h"
#include "types.h"
#include "funcs.h"
#include "systick.h"

extern volatile EEPROM_data_t cfg;

uint32_t sense_current = 0;
uint32_t sense_voltage = 0;
uint32_t battery_voltage = 0;
uint32_t sense_temperatureC = 0;
bool  sense_newData = false;
uint16_t adc_raw_voltage = 0;
uint16_t adc_raw_current = 0;
uint16_t adc_raw_temperature = 0;
uint16_t adc_raw_current_filtered = 0;

pidloop_t current_pid = {
    .Kp = 40,                // can be modified later through user configuration
    .Ki = 0,                 // can be modified later through user configuration
    .Kd = 100,               // can be modified later through user configuration
    .integral_limit = 20000, // cannot be modified later
    .output_limit   = 100000 // cannot be modified later
    // values are large, but the output will be divided later
};
uint16_t current_limit_duty = 0;

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

        uint16_t tempC =
            #if defined(STMICRO)
                __LL_ADC_CALC_TEMPERATURE(3300, adc_raw_temperature, LL_ADC_RESOLUTION_12B)
            #elif defined(ARTERY)
                (12600 - (int32_t)adc_raw_temperature * 33000 / 4096) / - 42 + 15
            #endif
                ;
        if (sense_temperatureC == 0) {
            sense_temperatureC = tempC;
        }
        sense_temperatureC = fi_lpf(sense_temperatureC, tempC, filter_const);

        sense_voltage = fi_lpf(sense_voltage, ((((adc_raw_voltage * 3300) / 4095) * cfg.voltage_divider) / 10), filter_const);

        adc_raw_current_filtered = fi_lpf(adc_raw_current_filtered, adc_raw_current * 10, filter_const);
        sense_current = ((adc_raw_current_filtered * 3300 / 41) - (cfg.current_offset * 1000)) / (cfg.current_scale);

        return true;
    }
    return false;
}

void current_limit_task(void)
{
    static uint32_t last_time = 0;
    if (cfg.current_limit <= 0) { // user disabled the limit
        current_limit_duty = 0;
        return;
    }
    // calculations needs to happen at 1 kHz intervals, since the PID calculation was taken from AM32 code
    uint32_t now = millis();
    if (last_time == now) {
        return;
    }
    last_time = now;
    current_limit_duty -= pid_calc(&current_pid, sense_current, cfg.current_limit) / 10000;

    const int32_t duty_min = DEAD_TIME;
    if (current_limit_duty < duty_min) {
        current_limit_duty = duty_min;
    }
    else if (current_limit_duty > cfg.pwm_period) {
        current_limit_duty = cfg.pwm_period;
    }
}

void load_config_pid(void)
{
    // the actual loop constants are mapped from a user specified range
    // this needs to account for the PWM period, so it needs to be recalculated, otherwise if the period changes, the PID won't behave the same
    current_pid.Kp    = fi_map(cfg.currlim_kp, 0, 100, 0, cfg.pwm_period / 40, false); // for a period of 2000, this gives a kp of 40, which is ideal
    current_pid.Ki    = cfg.currlim_ki;
    current_pid.Kd    = fi_map(cfg.currlim_kd, 0, 100, 0, cfg.pwm_period / 20, false); // for a period of 2000, this gives a kp of 100, which is ideal
    current_limit_duty = cfg.pwm_period;
}

uint8_t  batt_cell_cnt         = 0;
uint32_t batt_starting_voltage = 0;
uint32_t batt_max_voltage      = 0;
uint32_t voltage_limit         = 0; // specified in millivolts
static uint32_t prev_voltage   = 0;

void battery_task(void)
{
    if (voltage_limit != 0) { // once set, this function never does anything ever again
        return;
    }
    if (cfg.voltage_limit <= 0) {
        return;
    }
    if (cfg.voltage_limit > cfg.cell_max_volt) {
        voltage_limit = cfg.voltage_limit;
        return;
    }

    if (batt_cell_cnt <= 0 || batt_starting_voltage <= 0 || voltage_limit <= 0)
    {
        batt_max_voltage = sense_voltage > batt_max_voltage ? sense_voltage : batt_max_voltage;
        if (sense_voltage < prev_voltage || millis() >= BATTERY_RISE_MAX_TIME) { // calculate the cell count when the voltage stops rising, or when enough time passed
            batt_starting_voltage = batt_max_voltage;
            batt_cell_cnt = batt_starting_voltage / (cfg.cell_max_volt + 100);
            batt_cell_cnt += 1;
            voltage_limit = batt_cell_cnt * cfg.voltage_limit;

            dbg_printf("batt task t=%lums, cnt %u , maxv %u , v-lim %u\r\n", millis(), batt_cell_cnt, batt_max_voltage, voltage_limit);
        }
    }
    prev_voltage = sense_voltage;
}

#ifndef RELEASE_BUILD
void battery_reset(void)
{
    batt_cell_cnt         = 0;
    batt_starting_voltage = 0;
    batt_max_voltage      = 0;
    voltage_limit         = 0;
    prev_voltage          = 0;
}
#endif