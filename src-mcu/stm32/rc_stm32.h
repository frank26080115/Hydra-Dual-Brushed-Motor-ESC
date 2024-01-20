#include "rc.h"
#include "mcu.h"

// on the STM32 platform, there are two ways of measuring a RC pulse
//  * with input capture timer
//  * with a plain GPIO ISR, and a simple continuous timer for time stamping

class RcPulse_STM32 : public RcChannel
{
    public:
        RcPulse_STM32(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin);

    protected:
        TIM_TypeDef* _tim;
        GPIO_TypeDef* _gpio;
        uint32_t _pin;
};

class RcPulse_InputCap : public RcPulse_STM32
{
    public:
        RcPulse_InputCap(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin, uint32_t chan);
        virtual void init(void);
        virtual void task(void);
        virtual uint16_t read(void);
        virtual bool is_alive(void);
        virtual bool has_new(bool clr);

    protected:
        uint32_t _chan;
};

class RcPulse_GpioIsr : public RcPulse_STM32
{
    public:
        RcPulse_GpioIsr(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin);
        virtual void init(void);
        virtual void task(void);
        virtual uint16_t read(void);
        virtual bool is_alive(void);
        virtual bool has_new(bool clr);
};

extern bool ictimer_modeIsPulse;

RcPulse_GpioIsr* rc_makeGpioInput(void);
