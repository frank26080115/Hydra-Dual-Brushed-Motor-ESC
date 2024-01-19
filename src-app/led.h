#pragma once

#include "mcu.h"

#ifdef __cplusplus
extern "C" {
#endif

void led_init(void); // call to initialize IO and state machine
void led_task(void); // call continously to perform blinking

void ledhw_init(void); // MCU specific implementation

void ledblink_boot(void);
void ledblink_boot2(void);
void ledblink_disarmed(void);
void ledblink_armed_stopped(void);
void ledblink_moving(int8_t);
void ledblink_cli(void);

#ifdef __cplusplus
}
#endif
