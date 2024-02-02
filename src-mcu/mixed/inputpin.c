#include "inputpin.h"
#include "stm32_at32_compat.h"

void inp_init(void)
{
    #if defined(STMICRO)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = INPUT_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(INPUT_PIN_PORT, &GPIO_InitStruct);
    #elif defined(ARTERY)
    gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_INPUT, GPIO_PULL_NONE, INPUT_PIN);
    #else
    #error
    #endif
}

void inp_pullUp(void) {
    LL_GPIO_SetPinPull(INPUT_PIN_PORT, INPUT_PIN, LL_GPIO_PULL_UP);
}

void inp_pullDown(void) {
    LL_GPIO_SetPinPull(INPUT_PIN_PORT, INPUT_PIN, LL_GPIO_PULL_DOWN);
}

bool inp_read(void) {
    return LL_GPIO_IsInputPinSet(INPUT_PIN_PORT, INPUT_PIN);
}

#define INP2_GPIO    GPIOB
#define INP2_PIN     LL_GPIO_PIN_6

void inp2_init(void)
{
    #if defined(STMICRO)
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = INP2_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(INP2_GPIO, &GPIO_InitStruct);
    #elif defined(ARTERY)
    gpio_mode_QUICK(INP2_GPIO, GPIO_MODE_INPUT, GPIO_PULL_NONE, INP2_PIN);
    #else
    #error
    #endif
}

void inp2_pullUp(void) {
    LL_GPIO_SetPinPull(INP2_GPIO, INP2_PIN, LL_GPIO_PULL_UP);
}

void inp2_pullDown(void) {
    LL_GPIO_SetPinPull(INP2_GPIO, INP2_PIN, LL_GPIO_PULL_DOWN);
}

bool inp2_read(void) {
    return LL_GPIO_IsInputPinSet(INP2_GPIO, INP2_PIN);
}
