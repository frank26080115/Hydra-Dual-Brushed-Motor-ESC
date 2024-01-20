#pragma once

#include "main.h"
#include "eeprom.h"

extern uint32_t eeprom_save_time;
extern EEPROM_data_t cfg;

bool eeprom_verify_checksum(uint8_t* ptr8);
bool eeprom_load_or_default(void);
void eeprom_save(void);
void eeprom_save_if_needed(void);
void eeprom_mark_dirty(void);
bool eeprom_user_edit(char* str, int32_t* vptr);
