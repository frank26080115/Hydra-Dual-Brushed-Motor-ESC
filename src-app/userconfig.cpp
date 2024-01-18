#include "main.h"
#include "userconfig.h"

#define FOOL_AM32 \
    .fool_am32_bootloader_0   = 0x01, \
    .fool_am32_bootloader_1   = 0x01, \
    .fool_am32_bootloader_2   = 0x08, \
    .fool_am32_eeprom_layout  = 0x0A, \
    .fool_am32_version_major  = 1,    \
    .fool_am32_version_minor  = 99,   \
    .fool_am32_name           = {'F', 'L', 'O', 'O', 'R', 'I', 'T', '\0', }, \



// this stores a default settings copy in flash, somewhere inside the application flash memory
const EEPROM_data_t default_eeprom = {
    FOOL_AM32

    .magic              = 0xDEADBEEF,
    .version_major      = VERSION_MAJOR,
    .version_eeprom     = VERSION_EEPROM,

    .voltage_split_mode = VSPLITMODE_BOOST_ALWAYS,
    .input_mode         = INPUTMODE_RC,
    .phase_map          = 0,
    .baud               = 420000,

    .channel_0          = 1,
    .channel_1          = 2,
    .channel_mode       = 5,

    .rc_mid             = 1500,
    .rc_range           = 500,
    .rc_deadzone        = 10,

    .pwm_reload         = 1999,
    .pwm_headroom       = 50,

    .braking            = true,
    .chan_swap          = false,
    .flip_0             = false,
    .flip_1             = false,
    .tied               = false,
    .arm_duration       = 100,
    .temperature_limit  = 0,
    .current_limit      = 0,
};

// this stores a copy in the flash region allocated for EEPROM, this is writable
__attribute__((__section__(".EEPROM")))
const EEPROM_data_t cfge = {
    FOOL_AM32
};
uint32_t cfg_addr = (uint32_t)(&cfge);

// this stores the config in RAM
EEPROM_data_t cfg;

const EEPROM_item_t cfg_items[] = {
    { .name = "vsplitmode"   , .ptr = (uint32_t)&(cfge.voltage_split_mode ), .size = sizeof(cfge.voltage_split_mode  ), },
    { .name = "inputmode"    , .ptr = (uint32_t)&(cfge.input_mode         ), .size = sizeof(cfge.input_mode          ), },
    { .name = "phasemap"     , .ptr = (uint32_t)&(cfge.phase_map          ), .size = sizeof(cfge.phase_map           ), },
    { .name = "baud"         , .ptr = (uint32_t)&(cfge.baud               ), .size = sizeof(cfge.baud                ), },
    { .name = "channel_0"    , .ptr = (uint32_t)&(cfge.channel_0          ), .size = sizeof(cfge.channel_0           ), },
    { .name = "channel_1"    , .ptr = (uint32_t)&(cfge.channel_1          ), .size = sizeof(cfge.channel_1           ), },
    { .name = "channel_mode" , .ptr = (uint32_t)&(cfge.channel_mode       ), .size = sizeof(cfge.channel_mode        ), },
    { .name = "rc_mid"       , .ptr = (uint32_t)&(cfge.rc_mid             ), .size = sizeof(cfge.rc_mid              ), },
    { .name = "rc_range"     , .ptr = (uint32_t)&(cfge.rc_range           ), .size = sizeof(cfge.rc_range            ), },
    { .name = "rc_deadzone"  , .ptr = (uint32_t)&(cfge.rc_deadzone        ), .size = sizeof(cfge.rc_deadzone         ), },
    { .name = "pwm_reload"   , .ptr = (uint32_t)&(cfge.pwm_reload         ), .size = sizeof(cfge.pwm_reload          ), },
    { .name = "pwm_headroom" , .ptr = (uint32_t)&(cfge.pwm_headroom       ), .size = sizeof(cfge.pwm_headroom        ), },
    { .name = "braking"      , .ptr = (uint32_t)&(cfge.braking            ), .size = sizeof(cfge.braking             ), },
    { .name = "chanswap"     , .ptr = (uint32_t)&(cfge.chan_swap          ), .size = sizeof(cfge.chan_swap           ), },
    { .name = "flip0"        , .ptr = (uint32_t)&(cfge.flip_0             ), .size = sizeof(cfge.flip_0              ), },
    { .name = "flip1"        , .ptr = (uint32_t)&(cfge.flip_1             ), .size = sizeof(cfge.flip_1              ), },
    { .name = "tied"         , .ptr = (uint32_t)&(cfge.tied               ), .size = sizeof(cfge.tied                ), },
    { .name = "armdur"       , .ptr = (uint32_t)&(cfge.arm_duration       ), .size = sizeof(cfge.arm_duration        ), },
    { .name = "templim"      , .ptr = (uint32_t)&(cfge.temperature_limit  ), .size = sizeof(cfge.temperature_limit   ), },
    { .name = "currlim"      , .ptr = (uint32_t)&(cfge.current_limit      ), .size = sizeof(cfge.current_limit       ), },
    { .name = {0}, .ptr = 0, .size = 0, }, // indicate end of list
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

bool eeprom_item_strcmp(const char* usr_inp, const char* table_item)
{
    int slen = strlen(usr_inp);
    int i;
    for (i = 0; i < slen; i++)
    {
        char inpc = usr_inp[i];
        char x = table_item[i];
        if (inpc <= ' ' || inpc >= '~') {
            inpc = 0;
        }
        if (x <= ' ' || x >= '~') {
            x = 0;
        }
        if (inpc == 0 && x == 0) {
            return true;
        }
        if (inpc >= 'A' && inpc <= 'Z') {
            inpc -= 'A';
            inpc += 'a';
        }
        if (x >= 'A' && x <= 'Z') {
            x -= 'A';
            x += 'a';
        }
        if (inpc != x) {
            return false;
        }
    }
    return true;
}

bool eeprom_user_edit(const char* itm, const char* arg, int32_t* vptr)
{
    int i;
    for (i = 0; ; i++)
    {
        EEPROM_item_t* desc = (EEPROM_item_t*)&(cfg_items[i]);
        if (desc->ptr == 0) {
            return false;
        }
        if (eeprom_item_strcmp(itm, (const char*)desc->name))
        {
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

void eeprom_print_all(void)
{
    char buff[128];
    int i;
    for (i = 0; ; i++)
    {
        EEPROM_item_t* desc = (EEPROM_item_t*)&(cfg_items[i]);
        if (desc->ptr == 0) {
            return;
        }
        uint32_t ptrstart = (uint32_t)&cfge;
        uint32_t itmidx = desc->ptr - ptrstart;
        itmidx += (uint32_t)&cfg;
        uint8_t* ptr8 = (uint8_t*)&cfg;
        int32_t v = 0;
        memcpy(&v, &(ptr8[itmidx]), desc->size);
        sprintf(buff, "%s %li\r\n", desc->name, v);
        // TODO: send out serial port
    }
}
