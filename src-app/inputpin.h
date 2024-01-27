#pragma once

#include "main.h"
#include "mcu.h"

/*
functions here must be implemented in HAL
to manipulate the input pin

these functions are only used to check if CLI needs to activate
after that, the input pin either becomes a timer capture pin or a UART pin
*/

#ifdef __cplusplus
extern "C" {
#endif

void inp_init(void);
void inp_pullUp(void);
void inp_pullDown(void);
bool inp_read(void);

void inp2_init(void);
void inp2_pullUp(void);
void inp2_pullDown(void);
bool inp2_read(void);

#ifdef __cplusplus
}
#endif
