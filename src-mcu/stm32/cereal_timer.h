#pragma once
#include "stm32.h"
#include "cereal.h"

class Cereal_TimerBitbang : public Cereal
{
    public:
        Cereal_TimerBitbang(uint8_t id, TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, uint32_t pin);

        virtual void begin(uint32_t baud);

    private:
        TIM_TypeDef* _tim;
        GPIO_TypeDef* _gpio;
        uint32_t _pin;
};

extern void rc_ic_tim_init(void);
