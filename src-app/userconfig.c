#include "main.h"
#include "userconfig.h"
#include "systick.h"
#include "sense.h"
#include <string.h>
#include <stdlib.h>

#ifndef DEFAULT_INPUT_MODE
#define DEFAULT_INPUT_MODE     INPUTMODE_RC
#endif

// the bootloader of AM32 will check the 3rd byte for a version number, and erase the entire EEPROM if it doesn't like it
// https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/7

#if defined(MCU_F051)
#define FOOL_AM32_BOOTLOADER_VERSION 0x08
#else
#define FOOL_AM32_BOOTLOADER_VERSION 0x00
#endif

#define FOOL_AM32                     \
    .fool_am32_bootloader_0   = 0x01, \
    .fool_am32_bootloader_1   = 0x01, \
    .fool_am32_bootloader_version = FOOL_AM32_BOOTLOADER_VERSION, \
    .fool_am32_eeprom_layout  = 0x0A, \
    .fool_am32_version_major  = 1,    \
    .fool_am32_version_minor  = 99,   \
    .fool_am32_name           = { 'H', 'Y', 'D', 'R', 'A', '\0', }, \

#ifdef RELEASE_BUILD
// this copy of the config needs to remain unmodified, to avoid mistakes in configuration during a release build
#include "default_config.h"
#else
// this copy of the config can be modified for testing purposes, for the release factory-default config, see "default_config.h"
// this stores a default settings copy in flash, somewhere inside the application flash memory
// this is read-only, and can never be corrupted. it is used for factory-reset
const EEPROM_data_t default_eeprom __attribute__((aligned(4))) = {
    FOOL_AM32

    .magic              = EEPROM_MAGIC,
    .version_major      = VERSION_MAJOR,
    .version_minor      = VERSION_MINOR,
    .version_eeprom     = VERSION_EEPROM,

    .voltage_split_mode = 0,
    .load_balance       = false,
    .input_mode         = INPUTMODE_CRSF,
    .tank_arcade_mix    = false,
    .phase_map          = 1,
    .baud               = 0,

    .voltage_divider    = TARGET_VOLTAGE_DIVIDER,
    .current_offset     = CURRENT_OFFSET,
    .current_scale      = MILLIVOLT_PER_AMP,
    .adc_filter         = ADC_FILTER_DEFAULT,

    .channel_1          = 1,
    .channel_2          = 2,
    .channel_mode       = 3,
    .channel_brake      = 4,

    .rc_mid             = 1500,
    .rc_range           = 500,
    .rc_deadzone        = 10,

    .pwm_period         = PWM_DEFAULT_PERIOD,
    .pwm_deadtime       = PWM_DEFAULT_DEADTIME,

    .braking            = false,
    .chan_swap          = false,
    .flip_1             = false,
    .flip_2             = false,
    .tied               = false,
    .arm_duration       = RC_ARMING_CNT_REQ,
    .disarm_timeout     = RC_DISARM_TIMEOUT,
    .temperature_limit  = 0,
    .current_limit      = 0,
    .voltage_limit      = 0,
    .cell_max_volt      = 4350,
    .lowbatt_stretch    = 500,

    .currlim_kp         = 100,
    .currlim_ki         = 0,
    .currlim_kd         = 100,

    .dirpwm_chancfg_A   = 0,
    .dirpwm_chancfg_B   = 0,
    .dirpwm_chancfg_C   = 0,

    .tone_volume        = TONE_DEF_VOLUME,

    .write_cnt          = 0,
    .boot_log           = 0,
    .magic_end          = EEPROM_MAGIC,
};
#endif

// this stores a copy in the flash region allocated for EEPROM, this is writable (but not like RAM)
__attribute__((__section__(".eeprom")))
const EEPROM_data_t cfge = {
    FOOL_AM32
    .magic      = EEPROM_MAGIC,
    .write_cnt  = 0,
    .boot_log   = 0,
    .magic_end  = EEPROM_MAGIC,
};
#define cfg_addr    ((uint32_t)(&cfge))

// this stores the config in RAM
volatile EEPROM_data_t cfg __attribute__((aligned(4)));
// WARNING WARNING WARNING
// for some reason, this particular declaration is prone to being not 32 bit aligned, causing hard-faults, hence why the explicit alignment attribute
// please see https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/3 for more explanation

bool eeprom_has_loaded = false;

#ifdef ENABLE_COMPILE_CLI

#define DCLR_ITM(__a, __b)        { .name = __a, .ptr = (uint32_t)&(cfge.__b ), .size = sizeof(cfge.__b ), }

const EEPROM_item_t cfg_items[] __attribute__((aligned(4))) = {
    DCLR_ITM("vsplitmode"     , voltage_split_mode),
    DCLR_ITM("loadbal"        , load_balance      ),
    DCLR_ITM("inputmode"      , input_mode        ),
    DCLR_ITM("tankmix"        , tank_arcade_mix   ),
    DCLR_ITM("phasemap"       , phase_map         ),
    DCLR_ITM("baud"           , baud              ),
    DCLR_ITM("channel_1"      , channel_1         ),
    DCLR_ITM("channel_2"      , channel_2         ),
    DCLR_ITM("channel_mode"   , channel_mode      ),
    DCLR_ITM("channel_brake"  , channel_brake     ),
    DCLR_ITM("rc_mid"         , rc_mid            ),
    DCLR_ITM("rc_range"       , rc_range          ),
    DCLR_ITM("rc_deadzone"    , rc_deadzone       ),
    DCLR_ITM("pwm_period"     , pwm_period        ),
    DCLR_ITM("pwm_deadtime"   , pwm_deadtime      ),
    #ifndef PWM_ENABLE_BRIDGE
    DCLR_ITM("braking"        , braking           ),
    #endif
    DCLR_ITM("chanswap"       , chan_swap         ),
    DCLR_ITM("flip1"          , flip_1            ),
    DCLR_ITM("flip2"          , flip_2            ),
    DCLR_ITM("tied"           , tied              ),
    DCLR_ITM("armdur"         , arm_duration      ),
    DCLR_ITM("disarmtime"     , disarm_timeout    ),
    DCLR_ITM("templim"        , temperature_limit ),
    DCLR_ITM("currlim"        , current_limit     ),
    DCLR_ITM("voltlim"        , voltage_limit     ),
    DCLR_ITM("cellmaxvolt"    , cell_max_volt     ),
    DCLR_ITM("lowbattstretch" , lowbatt_stretch   ),
    DCLR_ITM("voltdiv"        , voltage_divider   ),
    DCLR_ITM("curroffset"     , current_offset    ),
    DCLR_ITM("currscale"      , current_scale     ),
    DCLR_ITM("adcfilter"      , adc_filter        ),
    DCLR_ITM("curlimkp"       , currlim_kp        ),
    DCLR_ITM("curlimki"       , currlim_ki        ),
    DCLR_ITM("curlimkd"       , currlim_kd        ),
    DCLR_ITM("dirpwm_1"       , dirpwm_chancfg_A  ),
    DCLR_ITM("dirpwm_2"       , dirpwm_chancfg_B  ),
    DCLR_ITM("dirpwm_3"       , dirpwm_chancfg_C  ),
    #if defined(DISABLE_LED) || defined(ENABLE_TONE)
    DCLR_ITM("tonevol"        , tone_volume       ),
    #endif
    { .ptr = 0, .size = 0, }, // indicate end of list
};

#endif

#ifndef RELEASE_BUILD
uint8_t eeprom_error_log = 0;
#endif

EEPROM_chksum_t eeprom_checksum(uint8_t* data, int len)
{
    #if 0
    // fletcher 16 https://en.wikipedia.org/wiki/Fletcher%27s_checksum
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    int index;

    for (index = 0; index < len; index++) {
        sum1 = (sum1 + data[index]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }

    return (sum2 << 8) | sum1;
    #else
    // less code size if we borrow the CRSF CRC function
    return crsf_crc8((const uint8_t *)data, len);
    #endif
}

uint8_t eeprom_get_bootloader_ver(void)
{
    #ifndef DEVELOPMENT_BOARD
    uint8_t* blv_ptr = (uint8_t*)(0x80000C0);
    return *blv_ptr;
    #else
    return 0;
    #endif
}

bool eeprom_verify_checksum(uint32_t* ptr8)
{
    volatile EEPROM_data_t* ptre = (volatile EEPROM_data_t*)ptr8; // https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/4
    uint8_t* start_addr = (uint8_t*)(&(ptre->magic));
    uint8_t* end_addr   = (uint8_t*)(&(ptre->chksum));
    volatile EEPROM_chksum_t calculated_chksum = eeprom_checksum(start_addr, (int)(((uint32_t)end_addr) - ((uint32_t)start_addr)));
    if (calculated_chksum != ptre->chksum) {
        dbg_printf("ERR: EEPROM checksum does not match (0x%02X != 0x%02X)\r\n", calculated_chksum, ptre->chksum);
    }
    return calculated_chksum == ptre->chksum;
}

bool eeprom_verify_header(uint32_t* ptr8)
{
    volatile EEPROM_data_t* ptre = (volatile EEPROM_data_t*)ptr8;
    bool ret = true;
    if (ptre->magic != EEPROM_MAGIC) {
        ret = false;
        dbg_printf("ERR: EEPROM magic does not match (0x%08X)\r\n", ptre->magic);
        #ifndef RELEASE_BUILD
        eeprom_error_log |= 0x02;
        #endif
    }
    if (ptre->version_eeprom != VERSION_EEPROM) {
        ret = false;
        dbg_printf("ERR: EEPROM version does not match (%u != %u)\r\n", ptre->version_eeprom, VERSION_EEPROM);
        #ifndef RELEASE_BUILD
        eeprom_error_log |= 0x04;
        #endif
    }
    #ifndef DEVELOPMENT_BOARD
    if (ptre->fool_am32_bootloader_version != eeprom_get_bootloader_ver()) {
        ret = false;
        dbg_printf("ERR: bootloader version does not match (%u != %u)\r\n", ptre->fool_am32_bootloader_version, eeprom_get_bootloader_ver());
        #ifndef RELEASE_BUILD
        eeprom_error_log |= 0x08;
        #endif
    }
    #endif
    return ret;
}

#if defined(ENABLE_COMPILE_CLI) && !defined(RELEASE_BUILD)
int eeprom_quick_validate(void)
{
    bool x = eeprom_verify_checksum((uint32_t*)cfg_addr);
    if (!x) {
        return 1;
    }
    memcpy((void*)&cfg, (void*)cfg_addr, sizeof(EEPROM_data_t));
    if (cfg.magic != EEPROM_MAGIC) {
        return 2;
    }
    if (cfg.version_eeprom != VERSION_EEPROM) {
        return 3;
    }
    return 0;
}
#endif

bool eeprom_load_or_default(void)
{
    bool x;

    x = eeprom_verify_header((uint32_t*)cfg_addr);

    x &= eeprom_verify_checksum((uint32_t*)cfg_addr);
    if (x == false) {
        #ifndef RELEASE_BUILD
        eeprom_error_log |= 0x01;
        #endif
    }

    if (x)
    {
        dbg_printf("EEPROM is valid\r\n");
        memcpy((void*)&cfg, (void*)cfg_addr, sizeof(EEPROM_data_t));
        eeprom_has_loaded = true;

        #ifndef RELEASE_BUILD
        if (cfg.boot_log != 0) {
            dbg_printf("EEPROM clearing stale boot log\r\n");
            cfg.boot_log = 0;
            eeprom_save();
        }
        #endif

        return true;
    }
    else
    {
        dbg_printf("EEPROM is invalid\r\n");
        #ifdef DEVELOPMENT_BOARD
        dbg_hexdump((uint32_t*)&cfg, sizeof(EEPROM_data_t));
        #endif

        eeprom_factory_reset();
        #ifdef DEVELOPMENT_BOARD
        bool x2 = eeprom_verify_checksum((uint32_t*)cfg_addr);
        if (x2 == false) {
            dbg_printf("ERROR: EEPROM is invalid even after factory reset\r\n");
        }
        #endif
        return false;
    }
}

void eeprom_load_defaults(void)
{
    memcpy((void*)&cfg, &default_eeprom, sizeof(EEPROM_data_t));
    eeprom_has_loaded = true;
}

void eeprom_factory_reset(void)
{
    dbg_printf("EEPROM factory resetting\r\n");
    uint32_t write_cnt = cfg.write_cnt;
    eeprom_load_defaults();
    cfg.write_cnt = write_cnt;
    eeprom_save();
}

uint32_t eeprom_save_time;

void eeprom_save(void)
{
    cfg.magic           = EEPROM_MAGIC;
    cfg.version_major   = VERSION_MAJOR;
    cfg.version_minor   = VERSION_MINOR;
    cfg.version_eeprom  = VERSION_EEPROM;
    EEPROM_data_t* ptre = (EEPROM_data_t*)&cfg;
    uint8_t* start_addr = (uint8_t*)(&(ptre->magic));
    uint8_t* end_addr   = (uint8_t*)(&(ptre->chksum));
    uint32_t head_len   = ((uint32_t)start_addr) - ((uint32_t)ptre);
    EEPROM_chksum_t calculated_chksum = eeprom_checksum(start_addr, (int)(((uint32_t)end_addr) - ((uint32_t)start_addr)));
    ptre->chksum = calculated_chksum;
    memcpy((void*)&cfg, &default_eeprom, head_len); // ensures header is written

    cfg.write_cnt++;
    cfg.fool_am32_bootloader_version = eeprom_get_bootloader_ver(); // ensure header has proper bootloader version, so the bootloader doesn't erase the EEPROM
    #ifndef RELEASE_BUILD
    cfg.boot_log = eeprom_error_log;
    #endif

    eeprom_write((uint32_t*)&cfg, sizeof(EEPROM_data_t), cfg_addr); // commit to flash
    eeprom_save_time = 0; // remove dirty flag
    dbg_printf("EEPROM saved\r\n");
}

bool eeprom_save_if_needed(void)
{
    if (eeprom_save_time > 0) {
        uint32_t now = millis();
        if ((now - eeprom_save_time) > EEPROM_DIRTY_SAVE_TIME_MS) {
            eeprom_save();
            return true;
        }
    }
    return false;
}

void eeprom_mark_dirty(void)
{
    eeprom_save_time = millis();
}

void eeprom_delay_dirty(void)
{
    if (eeprom_save_time > 0) {
        eeprom_save_time = millis();
    }
}

extern uint32_t arm_pulses_required;
extern uint32_t disarm_timeout;
extern void pwm_set_period(uint32_t);
extern void pwm_set_deadtime(uint32_t);
extern void pwm_set_remap(uint8_t);
extern void pwm_set_loadbalance(bool);
extern void pwm_set_braking(bool);

void load_runtime_configs(void)
{
    arm_pulses_required = cfg.arm_duration;
    disarm_timeout      = cfg.disarm_timeout;
    pwm_set_braking    (cfg.braking);
    pwm_set_period     (cfg.pwm_period);
    pwm_set_deadtime   (cfg.pwm_deadtime);
    pwm_set_remap      (cfg.phase_map);
    pwm_set_loadbalance(cfg.load_balance);
    load_config_pid();
}

#ifdef ENABLE_COMPILE_CLI
uint32_t eeprom_idx_of_item(char* str, int* argi)
{
    uint32_t ret32 = 0;
    uint16_t* ret16 = (uint16_t*)&ret32;
    int i;

    for (i = 0; ; i++)
    {
        EEPROM_item_t* desc = (EEPROM_item_t*)&(cfg_items[i]);
        if (desc->ptr == 0) {
            break; // end of list, return
        }
        if (item_strcmp(str, (const char*)desc->name, argi))
        {
            uint32_t ptrstart = (uint32_t)&cfge;
            uint32_t itmidx = desc->ptr - ptrstart;
            // every item's offset is stored as an absolute address, the offset from the start gives it a relative address (byte index)
            ret16[0] = itmidx;
            ret16[1] = desc->size;
            break; // found, return after exiting loop
        }
    }
    return ret32;
}

bool eeprom_user_edit(char* str, int32_t* retv)
{
    int argi;
    uint32_t idxret = eeprom_idx_of_item(str, &argi);
    uint16_t* idxret16 = (uint16_t*)&idxret;
    if (idxret16[1] <= 0) {
        return false;
    }

    char* arg = (char*)&str[argi + 1];
    int32_t v = parse_integer((const char*)arg);
    uint16_t itmidx = idxret16[0];
    uint8_t* ptr8 = (uint8_t*)&cfg; // retarget to the struct in RAM that's actually writable
    memcpy(&(ptr8[itmidx]), &v, idxret16[1]); // variable word size write
    eeprom_mark_dirty();
    if (retv != NULL) {
        *retv = v;
    }
    return true;
}
#endif
