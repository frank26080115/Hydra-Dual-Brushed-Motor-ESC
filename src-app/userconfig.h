#pragma once

#include "main.h"
#include "eeprom.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t eeprom_save_time;
extern volatile EEPROM_data_t cfg;
extern bool eeprom_has_loaded;
extern uint32_t cfg_addr;

extern volatile uint32_t eeprom_unlock_key; // must be the magic word for eeprom_save to work

bool eeprom_verify_checksum(uint32_t* ptr8);
bool eeprom_load_or_default(void);
void eeprom_load_defaults(void);
void eeprom_save(void);
bool eeprom_save_if_needed(void);
void eeprom_factory_reset(void);
void eeprom_mark_dirty(void);
void eeprom_delay_dirty(void);
bool eeprom_user_edit(char* str, int32_t* retv);
void load_runtime_configs(void);

#ifdef __cplusplus
}
#endif
