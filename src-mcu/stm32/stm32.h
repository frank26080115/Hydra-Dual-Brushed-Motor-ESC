#pragma once

#include "targets.h"

#ifdef MCU_F051
#include "stm32f051x8.h"
#include "stm32f0xx.h"
#include "system_stm32f0xx.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_adc.h"
#endif

#ifdef MCU_F031
// TODO
#endif

#ifdef MCU_G071
#include "stm32g071xx.h"
#include "stm32g0xx.h"
#include "system_stm32g0xx.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_adc.h"
#endif