#include "rc.h"

// on the STM32 platform, there are two ways of measuring a RC pulse
//  * with input capture timer
//  * with a plain GPIO ISR, and a simple continuous timer for time stamping

class RcPulse_STM32 : public RcChannel
{
    public:
        virtual void init(void);

    protected:
        TIM_TypeDef* _tim;
        GPIO_TypeDef* _gpio;
        uint32_t _pin;
};

class RcPulse_InputCap : public RcPulse_STM32
{
    public:
        RcPulse_InputCap(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin);
        virtual void init(void);
};

class RcPulse_GpioIsr : public RcPulse_STM32
{
    public:
        RcPulse_GpioIsr(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin);
        virtual void init(void);
};

extern bool ictimer_modeIsPulse;
