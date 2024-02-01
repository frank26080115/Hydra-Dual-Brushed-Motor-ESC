#pragma once

#include "rc.h"
#include "mcu.h"
#include "debug_tools.h"

#define GPIOEXTI_TIMx              TMR6
#define GPIOEXTI_Pin               GPIO_PINS_6
#define GPIOEXTI_GPIO              GPIOB
#define GPIOEXTI_IRQHandler        EXINT15_4_IRQHandler
#define GPIOEXTI_IRQn              EXINT15_4_IRQn
#define GPIOEXTI_Port              SCFG_PORT_SOURCE_GPIOB
#define GPIOEXTI_Line              EXINT_LINE_6
#define GPIOEXTI_SYSCFG_Line       SCFG_PINS_SOURCE6
#define GPIOEXTI_TIM_IRQHandler    TMR6_GLOBAL_IRQHandler
#define GPIOEXTI_TIM_IRQn          TMR6_GLOBAL_IRQn
#define GPIO_RC_PULSE_OFFSET       0

class RcPulse_InputCap : public RcChannel
{
    public:
        RcPulse_InputCap(tmr_type* TIMx, gpio_type* GPIOx, uint32_t pin, uint32_t chan);
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
        tmr_type* _tim;
        gpio_type* _gpio;
        uint32_t _pin;
        uint32_t _chan;
};

class RcPulse_GpioIsr : public RcChannel
{
    public:
        RcPulse_GpioIsr(void);
        virtual void init(tmr_type* TIMx, gpio_type* GPIOx, uint32_t pin);
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
        tmr_type* _tim;
        gpio_type* _gpio;
        uint32_t _pin;
};

#ifdef ENABLE_COMPILE_CLI
extern bool ictimer_modeIsPulse;
#endif
