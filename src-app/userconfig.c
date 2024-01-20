#include "main.h"
#include "userconfig.h"

#define FOOL_AM32                     \
    .fool_am32_bootloader_0   = 0x01, \
    .fool_am32_bootloader_1   = 0x01, \
    .fool_am32_bootloader_2   = 0x08, \
    .fool_am32_eeprom_layout  = 0x0A, \
    .fool_am32_version_major  = 1,    \
    .fool_am32_version_minor  = 99,   \
    .fool_am32_name           = {'H', 'Y', 'D', 'R', 'A', '\0', }, \

// this stores a default settings copy in flash, somewhere inside the application flash memory
const EEPROM_data_t default_eeprom = {
    FOOL_AM32

    .magic              = 0xDEADBEEF,
    .version_major      = VERSION_MAJOR,
    .version_eeprom     = VERSION_EEPROM,

    .voltage_split_mode = VSPLITMODE_BOOST_ALWAYS,
    .input_mode         = INPUTMODE_RC,
    .phase_map          = 0,
    .baud               = 416666,

    .voltage_divider    = TARGET_VOLTAGE_DIVIDER,
    .current_offset     = CURRENT_OFFSET,
    .current_scale      = MILLIVOLT_PER_AMP,
    .adc_filter         = 100,

    .channel_0          = 1,
    .channel_1          = 2,
    .channel_mode       = 5,

    .rc_mid             = 1500,
    .rc_range           = 500,
    .rc_deadzone        = 10,

    .pwm_reload         = PWM_DEFAULT_AUTORELOAD,
    .pwm_headroom       = PWM_DEFAULT_HEAADROOM,

    .braking            = true,
    .chan_swap          = false,
    .flip_0             = false,
    .flip_1             = false,
    .tied               = false,
    .arm_duration       = 100,
    .temperature_limit  = 0,
    .current_limit      = 0,

    .currlim_kp         = 400,
    .currlim_ki         = 0,
    .currlim_kd         = 1000,
};

// this stores a copy in the flash region allocated for EEPROM, this is writable
__attribute__((__section__(".EEPROM")))
const EEPROM_data_t cfge = {
    FOOL_AM32
};
uint32_t cfg_addr = (uint32_t)(&cfge);

// this stores the config in RAM
EEPROM_data_t cfg;

#define DCLR_ITM(__a, __b)        { .name = __a, .ptr = (uint32_t)&(cfge.__b ), .size = sizeof(cfge.__b ), }

const EEPROM_item_t cfg_items[] = {
    DCLR_ITM("vsplitmode"   , voltage_split_mode),
    DCLR_ITM("inputmode"    , input_mode        ),
    DCLR_ITM("phasemap"     , phase_map         ),
    DCLR_ITM("baud"         , baud              ),
    DCLR_ITM("channel_0"    , channel_0         ),
    DCLR_ITM("channel_1"    , channel_1         ),
    DCLR_ITM("channel_mode" , channel_mode      ),
    DCLR_ITM("rc_mid"       , rc_mid            ),
    DCLR_ITM("rc_range"     , rc_range          ),
    DCLR_ITM("rc_deadzone"  , rc_deadzone       ),
    DCLR_ITM("pwm_reload"   , pwm_reload        ),
    DCLR_ITM("pwm_headroom" , pwm_headroom      ),
    DCLR_ITM("braking"      , braking           ),
    DCLR_ITM("chanswap"     , chan_swap         ),
    DCLR_ITM("flip0"        , flip_0            ),
    DCLR_ITM("flip1"        , flip_1            ),
    DCLR_ITM("tied"         , tied              ),
    DCLR_ITM("armdur"       , arm_duration      ),
    DCLR_ITM("templim"      , temperature_limit ),
    DCLR_ITM("currlim"      , current_limit     ),
    DCLR_ITM("voltdiv"      , voltage_divider   ),
    DCLR_ITM("curroffset"   , current_offset    ),
    DCLR_ITM("currscale"    , current_scale     ),
    DCLR_ITM("adcfilter"    , adc_filter        ),
    DCLR_ITM("curlimkp"     , currlim_kp        ),
    DCLR_ITM("curlimki"     , currlim_ki        ),
    DCLR_ITM("curlimkd"     , currlim_kd        ),
    { .ptr = 0, .size = 0, }, // indicate end of list
};

EEPROM_chksum_t checksum_fletcher16(uint8_t* data, int len)
{
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    int index;

    for (index = 0; index < len; index++) {
        sum1 = (sum1 + data[index]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
}

bool eeprom_verify_checksum(uint8_t* ptr8)
{
    EEPROM_data_t* ptre = (EEPROM_data_t*)ptr8;
    uint8_t* start_addr = (uint8_t*)(&(ptre->magic));
    uint8_t* end_addr   = (uint8_t*)(&(ptre->chksum));
    EEPROM_chksum_t calculated_chksum = checksum_fletcher16(start_addr, (int)(((uint32_t)end_addr) - ((uint32_t)start_addr)));
    return calculated_chksum == ptre->chksum;
}

bool eeprom_load_or_default(void)
{
    bool x = eeprom_verify_checksum((uint8_t*)cfg_addr);
    if (x) {
        memcpy(&cfg, (void*)cfg_addr, sizeof(EEPROM_data_t));
        return true;
    }
    else {
        memcpy(&cfg, &default_eeprom, sizeof(EEPROM_data_t));
        eeprom_save();
        return false;
    }
}

uint32_t eeprom_save_time;

void eeprom_save(void)
{
    EEPROM_data_t* ptre = (EEPROM_data_t*)&cfg;
    uint8_t* start_addr = (uint8_t*)(&(ptre->magic));
    uint8_t* end_addr   = (uint8_t*)(&(ptre->chksum));
    uint32_t head_len   = ((uint32_t)start_addr) - ((uint32_t)ptre);
    EEPROM_chksum_t calculated_chksum = checksum_fletcher16(start_addr, (int)(((uint32_t)end_addr) - ((uint32_t)start_addr)));
    ptre->chksum = calculated_chksum;
    memcpy(&cfg, &default_eeprom, head_len);                       // ensures header is written
    eeprom_write((uint8_t*)&cfg, sizeof(EEPROM_data_t), cfg_addr); // commit to flash
    eeprom_save_time = 0;
}

void eeprom_save_if_needed(void)
{
    if (eeprom_save_time > 0) {
        uint32_t now = millis();
        if ((now - eeprom_save_time) > EEPROM_DIRTY_SAVE_TIME_MS) {
            eeprom_save();
        }
    }
}

void eeprom_mark_dirty(void)
{
    eeprom_save_time = millis();
}

bool eeprom_user_edit(char* str, int32_t* vptr)
{
    int i;
    char* arg;

    for (i = 0; ; i++)
    {
        EEPROM_item_t* desc = (EEPROM_item_t*)&(cfg_items[i]);
        if (desc->ptr == 0) {
            return false;
        }
        if (item_strcmp(str, (const char*)desc->name))
        {
            strtok(str, " ");
            arg = strtok(NULL, " ");
            int32_t v;
            if (arg[0] == '0' && (arg[1] == 'x' || arg[1] == 'X')) {
                v = strtol(&arg[2], NULL, 16);
            }
            else {
                v = atoi(arg);
            }
            uint32_t ptrstart = (uint32_t)&cfge;
            uint32_t itmidx = desc->ptr - ptrstart;
            itmidx += (uint32_t)&cfg;
            uint8_t* ptr8 = (uint8_t*)&cfg;

            memcpy(&(ptr8[itmidx]), &v, desc->size);
            if (vptr != NULL) {
                *vptr = 0;
                memcpy(vptr, &v, desc->size);
            }
            return true;
        }
    }
    return false;
}
