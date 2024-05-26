#pragma once

#include "cereal.h"

enum
{
    TELEMSWAP_NO_NEED,
    TELEMSWAP_PA2_PA14,
    TELEMSWAP_PA14_BIDIR,
};

void telem_init(Cereal_USART* cer, uint8_t swap_mode);
void telem_crsfTask(void); // call this immediately after reception of a CRSF packet
void telem_mainTask(void); // call this from the main loop
