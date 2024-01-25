#pragma once

#include "rc.h"
#include "mcu.h"
#include "debug_tools.h"

#if defined(MCU_F051)
// PB6 is telemetry pin
#define GPIOEXTI_TIMx              TIM6
#define GPIOEXTI_Pin               LL_GPIO_PIN_6
#define GPIOEXTI_GPIO              GPIOB
#define GPIOEXTI_IRQHandler        EXTI4_15_IRQHandler
#define GPIOEXTI_IRQn              EXTI4_15_IRQn
#define GPIOEXTI_Port              LL_SYSCFG_EXTI_PORTB
#define GPIOEXTI_Line              LL_EXTI_LINE_6
#define GPIOEXTI_SYSCFG_Line       LL_SYSCFG_EXTI_LINE6
#define GPIOEXTI_TIM_IRQHandler    TIM6_IRQHandler
#define GPIOEXTI_TIM_IRQn          TIM6_IRQn
#define GPIO_RC_PULSE_OFFSET       0
#elif defined(MCU_G071)
// PB6 is telemetry pin
#define GPIOEXTI_TIMx              TIM6
#define GPIOEXTI_Pin               LL_GPIO_PIN_6
#define GPIOEXTI_GPIO              GPIOB
#define GPIOEXTI_IRQHandler        EXTI4_15_IRQHandler
#define GPIOEXTI_IRQn              EXTI4_15_IRQn
#define GPIOEXTI_Port              LL_EXTI_CONFIG_PORTB
#define GPIOEXTI_Line              LL_EXTI_LINE_6
#define GPIOEXTI_SYSCFG_Line       LL_EXTI_CONFIG_LINE6
#define GPIOEXTI_TIM_IRQHandler    TIM6_DAC_LPTIM1_IRQHandler
#define GPIOEXTI_TIM_IRQn          TIM6_DAC_LPTIM1_IRQn
#define GPIO_RC_PULSE_OFFSET       0
#endif

// on the STM32 platform, there are two ways of measuring a RC pulse
//  * with input capture timer
//  * with a plain GPIO ISR, and a simple continuous timer for time stamping

class RcPulse_InputCap : public RcChannel
{
    public:
        RcPulse_InputCap(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin, uint32_t chan);
        virtual void init(void);
        virtual void task(void);
        virtual int16_t read(void);
        virtual int16_t readRaw(void);
        virtual bool is_alive(void);
        virtual bool has_new(bool clr);
        virtual bool is_armed(void);
        virtual void disarm(void);
        #ifdef RC_LOG_JITTER
        virtual uint32_t readJitter(void);
        #endif

    protected:
        TIM_TypeDef* _tim;
        GPIO_TypeDef* _gpio;
        uint32_t _pin;
        uint32_t _chan;
};

class RcPulse_GpioIsr : public RcChannel
{
    public:
        RcPulse_GpioIsr(void);
        virtual void init(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin);
        virtual void task(void);
        virtual int16_t read(void);
        virtual int16_t readRaw(void);
        virtual bool is_alive(void);
        virtual bool has_new(bool clr);
        virtual bool is_armed(void);
        virtual void disarm(void);
        #ifdef RC_LOG_JITTER
        virtual uint32_t readJitter(void);
        #endif

    protected:
        TIM_TypeDef* _tim;
        GPIO_TypeDef* _gpio;
        uint32_t _pin;
};

#ifdef ENABLE_COMPILE_CLI
extern bool ictimer_modeIsPulse;
#endif
