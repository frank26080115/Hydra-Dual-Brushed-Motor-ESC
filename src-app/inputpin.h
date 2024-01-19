#pragma once

#include "main.h"
#include "mcu.h"

/*
functions here must be implemented in HAL
to manipulate the input pin
*/

#ifdef __cplusplus
extern "C" {
#endif

void inp_init(void);
void inp_pullUp(void);
void inp_pullDown(void);
bool inp_read(void);

#ifdef __cplusplus
}
#endif
