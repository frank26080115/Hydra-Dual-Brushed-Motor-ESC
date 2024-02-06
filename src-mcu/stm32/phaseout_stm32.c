#include "phaseout.h"

#ifndef PWM_ENABLE_BRIDGE
#ifdef USE_INVERTED_LOW
    #define LOW_BITREG_ON  BRR
    #define LOW_BITREG_OFF BSRR
#else
    #define LOW_BITREG_ON  BSRR
    #define LOW_BITREG_OFF BRR
#endif

#ifdef USE_INVERTED_HIGH
    #define HIGH_BITREG_OFF BSRR
#else
    #define HIGH_BITREG_OFF BRR
#endif

#else
#define LOW_BITREG_ON   BSRR
#define LOW_BITREG_OFF  BRR
#define HIGH_BITREG_ON  BSRR
#define HIGH_BITREG_OFF BRR
#endif

void pwm_init()
{
    LL_TIM_InitTypeDef       TIM_InitStruct     = {0};
    LL_TIM_OC_InitTypeDef    TIM_OC_InitStruct  = {0};
    LL_TIM_BDTR_InitTypeDef  TIM_BDTRInitStruct = {0};
    LL_GPIO_InitTypeDef      GPIO_InitStruct    = {0};

    TIM_InitStruct.Prescaler          = 0;
    TIM_InitStruct.CounterMode        = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload         = PWM_DEFAULT_PERIOD;
    TIM_InitStruct.ClockDivision      = LL_TIM_CLOCKDIVISION_DIV1;
    TIM_InitStruct.RepetitionCounter  = 0;
    LL_TIM_Init(PWMOUTTIMx, &TIM_InitStruct);
    LL_TIM_EnableARRPreload(PWMOUTTIMx);
//#ifdef MCU_F051
    LL_TIM_SetClockSource(PWMOUTTIMx, LL_TIM_CLOCKSOURCE_INTERNAL);
//#endif
    LL_TIM_OC_EnablePreload(PWMOUTTIMx, LL_TIM_CHANNEL_CH1);
#ifdef USE_SWAPPED_OUPUT
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM2;
#else
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
#endif
    TIM_OC_InitStruct.OCState      = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState     = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;

#if defined(MCU_F051)
#ifdef USE_INVERTED_HIGH
    TIM_OC_InitStruct.OCPolarity   = LL_TIM_OCPOLARITY_LOW;
    TIM_OC_InitStruct.OCIdleState  = LL_TIM_OCIDLESTATE_HIGH;
#else
    TIM_OC_InitStruct.OCPolarity   = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCIdleState  = LL_TIM_OCIDLESTATE_LOW;
#endif
#ifdef USE_INVERTED_LOW
    TIM_OC_InitStruct.OCNPolarity  = LL_TIM_OCPOLARITY_LOW;
    TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_HIGH;
#else
    TIM_OC_InitStruct.OCNPolarity  = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
#endif
#elif defined(MCU_G071)
    TIM_OC_InitStruct.OCPolarity   = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCIdleState  = LL_TIM_OCIDLESTATE_LOW;
    TIM_OC_InitStruct.OCNPolarity  = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
#else
#error
#endif
    LL_TIM_OC_Init(PWMOUTTIMx, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(PWMOUTTIMx, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_EnablePreload(PWMOUTTIMx, LL_TIM_CHANNEL_CH2);
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    LL_TIM_OC_Init(PWMOUTTIMx, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(PWMOUTTIMx, LL_TIM_CHANNEL_CH2);
    LL_TIM_OC_EnablePreload(PWMOUTTIMx, LL_TIM_CHANNEL_CH3);
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    LL_TIM_OC_Init(PWMOUTTIMx, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(PWMOUTTIMx, LL_TIM_CHANNEL_CH3);

#if 0

    LL_TIM_OC_EnablePreload(PWMOUTTIMx, LL_TIM_CHANNEL_CH4);
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    LL_TIM_OC_Init(PWMOUTTIMx, LL_TIM_CHANNEL_CH4, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(PWMOUTTIMx, LL_TIM_CHANNEL_CH4);

    #if defined(MCU_G071)
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH5);
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH5, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH5);
    #endif
#endif

    LL_TIM_SetTriggerOutput(PWMOUTTIMx, LL_TIM_TRGO_RESET);
    #ifdef MCU_G071
    LL_TIM_SetTriggerOutput2(PWMOUTTIMx, LL_TIM_TRGO2_RESET);
    #endif
    LL_TIM_DisableMasterSlaveMode(PWMOUTTIMx);

    TIM_BDTRInitStruct.OSSRState      = LL_TIM_OSSR_DISABLE;
    TIM_BDTRInitStruct.OSSIState      = LL_TIM_OSSI_DISABLE;
    TIM_BDTRInitStruct.LockLevel      = LL_TIM_LOCKLEVEL_OFF;
    TIM_BDTRInitStruct.DeadTime       = DEAD_TIME;
    TIM_BDTRInitStruct.BreakState     = LL_TIM_BREAK_DISABLE;
    TIM_BDTRInitStruct.BreakPolarity  = LL_TIM_BREAK_POLARITY_HIGH;
    #ifdef MCU_G071
    TIM_BDTRInitStruct.BreakFilter    = LL_TIM_BREAK_FILTER_FDIV1;
    TIM_BDTRInitStruct.BreakAFMode    = LL_TIM_BREAK_AFMODE_INPUT;
    TIM_BDTRInitStruct.Break2State    = LL_TIM_BREAK2_DISABLE;
    TIM_BDTRInitStruct.Break2Polarity = LL_TIM_BREAK2_POLARITY_HIGH;
    TIM_BDTRInitStruct.Break2Filter   = LL_TIM_BREAK2_FILTER_FDIV1;
    TIM_BDTRInitStruct.Break2AFMode   = LL_TIM_BREAK_AFMODE_INPUT;
    #endif
    TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_ENABLE; // LL_TIM_AUTOMATICOUTPUT_DISABLE;
    LL_TIM_BDTR_Init(PWMOUTTIMx, &TIM_BDTRInitStruct);
    PWMOUTTIMx->BDTR |= DEAD_TIME;

#ifdef MCU_G071
#ifdef PWM_ENABLE_BRIDGE
#define PHASE_C_GPIO_LOW           PHASE_C_GPIO_ENABLE
#define PHASE_B_GPIO_LOW           PHASE_B_GPIO_ENABLE
#define PHASE_A_GPIO_LOW           PHASE_A_GPIO_ENABLE
#define PHASE_C_GPIO_PORT_LOW      PHASE_C_GPIO_PORT_ENABLE
#define PHASE_B_GPIO_PORT_LOW      PHASE_B_GPIO_PORT_ENABLE
#define PHASE_A_GPIO_PORT_LOW      PHASE_A_GPIO_PORT_ENABLE

#define PHASE_C_GPIO_HIGH          PHASE_C_GPIO_PWM
#define PHASE_B_GPIO_HIGH          PHASE_B_GPIO_PWM
#define PHASE_A_GPIO_HIGH          PHASE_A_GPIO_PWM
#define PHASE_C_GPIO_PORT_HIGH     PHASE_C_GPIO_PORT_PWM
#define PHASE_B_GPIO_PORT_HIGH     PHASE_B_GPIO_PORT_PWM
#define PHASE_A_GPIO_PORT_HIGH     PHASE_A_GPIO_PORT_PWM
#endif // PWM_ENABLE_BRIDGE

#ifndef OPEN_DRAIN_PWM
#define PWM_OUTPUT_TYPE            LL_GPIO_OUTPUT_PUSHPULL
#else
#define PWM_OUTPUT_TYPE            LL_GPIO_OUTPUT_OPENDRAIN
#endif

#ifndef LOW_OUTPUT_TYPE
#define LOW_OUTPUT_TYPE            PWM_OUTPUT_TYPE
#endif

#ifndef HIGH_OUTPUT_TYPE
#define HIGH_OUTPUT_TYPE           PWM_OUTPUT_TYPE
#endif

#endif // MCU_G071

#ifdef MCU_F051
#ifdef USE_OPEN_DRAIN_LOW
#define LOW_OUTPUT_TYPE            LL_GPIO_OUTPUT_OPENDRAIN
#else
#define LOW_OUTPUT_TYPE            LL_GPIO_OUTPUT_PUSHPULL
#endif
#ifdef USE_OPEN_DRAIN_HIGH
#define HIGH_OUTPUT_TYPE           LL_GPIO_OUTPUT_OPENDRAIN
#else
#define HIGH_OUTPUT_TYPE           LL_GPIO_OUTPUT_PUSHPULL
#endif
#endif

    GPIO_InitStruct.Pin        = PHASE_A_GPIO_LOW;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LOW_OUTPUT_TYPE;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_2;
    LL_GPIO_Init(PHASE_A_GPIO_PORT_LOW, &GPIO_InitStruct);

    GPIO_InitStruct.Pin        = PHASE_B_GPIO_LOW;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LOW_OUTPUT_TYPE;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_2;
    LL_GPIO_Init(PHASE_B_GPIO_PORT_LOW, &GPIO_InitStruct);

    GPIO_InitStruct.Pin        = PHASE_C_GPIO_LOW;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LOW_OUTPUT_TYPE;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_2;
    LL_GPIO_Init(PHASE_C_GPIO_PORT_LOW, &GPIO_InitStruct);

    GPIO_InitStruct.Pin        = PHASE_A_GPIO_HIGH;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = HIGH_OUTPUT_TYPE;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_2;
    LL_GPIO_Init(PHASE_A_GPIO_PORT_HIGH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin        = PHASE_B_GPIO_HIGH;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = HIGH_OUTPUT_TYPE;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_2;
    LL_GPIO_Init(PHASE_B_GPIO_PORT_HIGH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin        = PHASE_C_GPIO_HIGH;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = HIGH_OUTPUT_TYPE;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_2;
    LL_GPIO_Init(PHASE_C_GPIO_PORT_HIGH, &GPIO_InitStruct);

#ifdef PWM_NEED_IRQ
    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 2);
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
#endif

    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1N);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2N);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);

    LL_TIM_EnableCounter(TIM1);
    LL_TIM_EnableAllOutputs(TIM1);

    LL_TIM_GenerateEvent_UPDATE(TIM1);
}

void pwm_set_period(uint32_t x)
{
    LL_TIM_SetAutoReload(PWMOUTTIMx, x);
}

void pwm_set_deadtime(uint32_t x)
{
    LL_TIM_OC_SetDeadTime(PWMOUTTIMx, x);
}

#ifdef PWM_NEED_IRQ
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_CC1(PWMOUTTIMx) == 1) {
        LL_TIM_ClearFlag_CC1(PWMOUTTIMx);
    }

    if (LL_TIM_IsActiveFlag_UPDATE(PWMOUTTIMx) == 1) {
        LL_TIM_ClearFlag_UPDATE(PWMOUTTIMx);
    }
}
#endif

#include "stm32_at32_compat.h"
#include "phaseout_shared.h"
