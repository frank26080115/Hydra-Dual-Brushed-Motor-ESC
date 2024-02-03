#include "debug_tools.h"

#if defined(DEBUG_PRINT)

extern int debug_writebuff(uint8_t* buf, int len);
extern int debug_printf(const char *format, va_list arg);

extern uint8_t cer_buff_3[CEREAL_BUFFER_SIZE];

void dbg_printf(const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    size_t ret = debug_printf(fmt, arg);
    va_end(arg);
    (void)ret;
    //return ret;
}

#endif

void dbg_button_init(void)
{
    #ifdef DEVELOPMENT_BOARD
    #if defined(MCU_F051)
    #define DBG_BUTTON_PIN   LL_GPIO_PIN_0
    #define DBG_BUTTON_PORT  GPIOA
    #define DBG_BUTTON_PULL  LL_GPIO_PULL_DOWN
    #elif defined(MCU_G071)
    #define DBG_BUTTON_PIN   LL_GPIO_PIN_13
    #define DBG_BUTTON_PORT  GPIOC
    #define DBG_BUTTON_PULL  LL_GPIO_PULL_UP
    #elif defined(MCU_AT421)
    #endif
    #if defined(STMICRO)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DBG_BUTTON_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    #if defined(MCU_F051)
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    #elif defined(MCU_G071)
    GPIO_InitStruct.Pull = DBG_BUTTON_PULL;
    #endif
    LL_GPIO_Init(DBG_BUTTON_PORT, &GPIO_InitStruct);
    #elif defined(ARTERY)
    //gpio_mode_QUICK(DBG_BUTTON_PORT, GPIO_MODE_INPUT, GPIO_PULL_NONE, DBG_BUTTON_PIN); // TODO change pull
    #endif
    #endif
}

bool dbg_read_btn(void)
{
    #ifdef DEVELOPMENT_BOARD
    #if defined(STMICRO)
    #if DBG_BUTTON_PULL == LL_GPIO_PULL_UP
    return LL_GPIO_IsInputPinSet(DBG_BUTTON_PORT, DBG_BUTTON_PIN) == 0;
    #else
    return LL_GPIO_IsInputPinSet(DBG_BUTTON_PORT, DBG_BUTTON_PIN) != 0;
    #endif
    #endif
    #else
    return false;
    #endif
}

#if defined(DEBUG_EVENTCNT)

static volatile uint32_t dbg_evnt_cnt[12] = {0};

void dbg_evntcnt_add(uint8_t id)
{
    dbg_evnt_cnt[id]++;
}

uint32_t dbg_evntcnt_get(uint8_t id)
{
    return dbg_evnt_cnt[id];
}

#endif

#if defined(DEBUG_PINTOGGLE)

static volatile bool was_high[2];

void dbg_pintoggle_init(void)
{
    #ifdef DEVELOPMENT_BOARD
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_3 | LL_GPIO_PIN_5;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    dbg_pinset(0, false);
    dbg_pinset(1, false);
    #endif
}

void dbg_pinset(uint8_t p, bool high)
{
    #ifdef DEVELOPMENT_BOARD
    if (high) {
        LL_GPIO_SetOutputPin(GPIOB, p == 0 ? LL_GPIO_PIN_3 : LL_GPIO_PIN_5);
    }
    else {
        LL_GPIO_ResetOutputPin(GPIOB, p == 0 ? LL_GPIO_PIN_3 : LL_GPIO_PIN_5);
    }
    #endif
    was_high[p] = high;
}

void dbg_pintoggle(uint8_t p)
{
    dbg_pinset(p, was_high[p] == false);
}

#endif

#if defined(STM32F051DISCO)
void dbg_switch_to_pwm(void)
{
    LL_GPIO_SetPinMode   (GPIOA, LL_GPIO_PIN_9 , LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode   (GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9 , LL_GPIO_AF_2);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_2);
}

void dbg_switch_to_cereal(void)
{
    LL_GPIO_SetPinMode   (GPIOA, LL_GPIO_PIN_9 , LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode   (GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_9 , LL_GPIO_AF_1);
    LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_10, LL_GPIO_AF_1);
}
#endif
