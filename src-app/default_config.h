// this file is meant to be included inline, do not include this file where it's not supposed to go

// this copy of the config is meant for release mode, do not edit it for willy-nilly testing

const EEPROM_data_t default_eeprom __attribute__((aligned(4))) = {
    FOOL_AM32

    .magic              = EEPROM_MAGIC,
    .version_major      = VERSION_MAJOR,
    .version_minor      = VERSION_MINOR,
    .version_eeprom     = VERSION_EEPROM,

    .voltage_split_mode = 0,
    .load_balance       = false,
    .input_mode         = DEFAULT_INPUT_MODE,
    .tank_arcade_mix    = false,
    .phase_map          = 1,
    .baud               = 0,

    .voltage_divider    = TARGET_VOLTAGE_DIVIDER,
    .current_offset     = CURRENT_OFFSET,
    .current_scale      = MILLIVOLT_PER_AMP,
    .adc_filter         = ADC_FILTER_DEFAULT,

    .channel_1          = 1,
    .channel_2          = 2,
    .channel_mode       = 0,

    .rc_mid             = 1500,
    .rc_range           = 500,
    .rc_deadzone        = 10,

    .pwm_period         = PWM_DEFAULT_PERIOD,
    .pwm_deadtime       = PWM_DEFAULT_DEADTIME,

    .braking            = true,
    .chan_swap          = false,
    .flip_1             = false,
    .flip_2             = false,
    .tied               = false,
    .arm_duration       = RC_ARMING_CNT_REQ,
    .disarm_timeout     = RC_DISARM_TIMEOUT,
    .temperature_limit  = 0,
    .current_limit      = 0,
    .voltage_limit      = 0,

    .currlim_kp         = 100,
    .currlim_ki         = 0,
    .currlim_kd         = 100,

    .dirpwm_chancfg_A   = 0,
    .dirpwm_chancfg_B   = 0,
    .dirpwm_chancfg_C   = 0,

    .tone_volume        = TONE_DEF_VOLUME,
};
