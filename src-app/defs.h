#pragma once

#include "conf.h"

// this file is used for definitions for constants, and renaming of definitions
// use for items that are #define that literally never have to change
// but is not a type definition

#define EEPROM_MAGIC    0xDEADBEEF

#ifndef CLI_ECHO
#ifdef DEVELOPMENT_BOARD
#define CLI_ECHO    true  // development boards use two wires for UART
#else
#define CLI_ECHO    false // most of the one-wire USB-links already echo, so do not echo ourselves
#endif
#endif
