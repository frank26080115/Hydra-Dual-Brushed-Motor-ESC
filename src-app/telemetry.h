#pragma once

#include "cereal.h"

void telemetry_init(void);
void telem_crsfTask(void); // call this immediately after reception of a CRSF packet
void telem_mainTask(void); // call this from the main loop
