#pragma once

#include "targets.h"

#if defined(MCU_F421)
#include "at32f421.h"
#include "at32f421_adc.h"
#include "at32f421_cmp.h"
#include "at32f421_crc.h"
#include "at32f421_crm.h"
#include "at32f421_debug.h"
#include "at32f421_def.h"
#include "at32f421_dma.h"
#include "at32f421_ertc.h"
#include "at32f421_exint.h"
#include "at32f421_flash.h"
#include "at32f421_gpio.h"
#include "at32f421_i2c.h"
#include "at32f421_misc.h"
#include "at32f421_pwc.h"
#include "at32f421_scfg.h"
#include "at32f421_spi.h"
#include "at32f421_tmr.h"
#include "at32f421_usart.h"
#include "at32f421_wdt.h"
#include "at32f421_wwdt.h"
#endif

#define GPIO_PIN_SWDIO LL_GPIO_PIN_13
#define GPIO_PIN_SWCLK LL_GPIO_PIN_14

#define CLK_CNT(rate) ((SystemCoreClock + ((rate) >> 1)) / (rate))

#ifdef __cplusplus
extern "C" {
#endif

extern void gpio_mode_QUICK(gpio_type* gpio_periph, uint32_t mode, uint32_t pull_up_down, uint32_t pin);

#ifdef __cplusplus
}
#endif
