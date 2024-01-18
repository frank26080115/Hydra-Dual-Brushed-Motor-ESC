#include "rc.h"

#if defined(MCU_F051) && (INPUT_PIN == LL_GPIO_PIN_4)
// PB6 is telemetry pin
#define EXTI_IRQHandler 
#endif

void

EXTI0_1_IRQHandler(void)
{

}
