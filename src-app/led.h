#pragma once

#include "mcu.h"

void led_init(void); // call to initialize IO and state machine
void led_task(void); // call continously to perform blinking

void ledblink_disarmed(void);
void ledblink_armed_stopped(void);
void ledblink_moving(void);
void ledblink_cli(void);
