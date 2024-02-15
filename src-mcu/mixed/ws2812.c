#include "mcu.h"
#include "ws2812.h"
#include "led.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef USE_LED_STRIP

#if defined(MCU_G071)
#include "ws2812_timerdma.h"
#elif defined(MCU_AT421)
#include "ws2812_bitbang.h"
#endif

#endif
