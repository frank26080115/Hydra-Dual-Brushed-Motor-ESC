#pragma once

#include <stdio.h>
#include <stdarg.h>

#include "conf.h"
#include "defs.h"

#ifdef DEBUG_PRINT

#ifdef __cplusplus
extern "C" {
#endif

int dbg_printf(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#else

#define dbg_printf(...) // nothing

#endif
