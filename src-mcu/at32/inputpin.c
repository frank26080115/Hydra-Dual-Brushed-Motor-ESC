#include "inputpin.h"

void inp_init(void)
{
    gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_INPUT, GPIO_PULL_NONE, INPUT_PIN);
}

void inp_pullUp(void) {
    gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_INPUT, GPIO_PULL_UP, INPUT_PIN);
}

void inp_pullDown(void) {
    gpio_mode_QUICK(INPUT_PIN_PORT, GPIO_MODE_INPUT, GPIO_PULL_DOWN, INPUT_PIN);
}

bool inp_read(void) {
    return gpio_input_data_bit_read(INPUT_PIN_PORT, INPUT_PIN);
}

#define INP2_GPIO    GPIOB
#define INP2_PIN     GPIO_PINS_6

void inp2_init(void)
{
    gpio_mode_QUICK(INP2_GPIO, GPIO_MODE_INPUT, GPIO_PULL_NONE, INP2_PIN);
}

void inp2_pullUp(void) {
    gpio_mode_QUICK(INP2_GPIO, GPIO_MODE_INPUT, GPIO_PULL_UP, INP2_PIN);
}

void inp2_pullDown(void) {
    gpio_mode_QUICK(INP2_GPIO, GPIO_MODE_INPUT, GPIO_PULL_DOWN, INP2_PIN);
}

bool inp2_read(void) {
    return gpio_input_data_bit_read(INP2_GPIO, INP2_PIN);
}
