#include "led.h"
#include <stddef.h>
#include "systick.h"

void led_init(void)
{
    ledhw_init();
}

#include "led_blink_patterns.h"

#ifdef ENABLE_LED_BLINK
uint16_t blink_idx;                       // index of the current frame in the pattern
uint8_t* blink_primary_pattern   = NULL;  // primary pattern is used to indicate current status, and loops forever
uint8_t* blink_secondary_pattern = NULL;  // secondary pattern is used to indicate events, and will end, switching back to primary pattern
uint32_t blink_tmr = 0;                   // forever running timer
uint8_t  blink_span = 0;                  // the time span of the current time frame
uint32_t blink_last_time = 0;             // the last time that the frame advanced
#endif

#if !defined(ENABLE_LED_BLINK) && !defined(USE_RGB_LED) && defined(LED_GPIO) && defined(LED_PIN)
static uint8_t led_state = 0;
#endif

void led_blink_set(uint8_t x)
{
    #ifdef USE_RGB_LED
    led_set(LED_GPIO_RED  , LED_PIN_RED  , BLINK_IS_RED(x)  , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_GREEN, LED_PIN_GREEN, BLINK_IS_GREEN(x), LED_IS_OPENDRAIN);
    led_set(LED_GPIO_BLUE , LED_PIN_BLUE , BLINK_IS_BLUE(x) , LED_IS_OPENDRAIN);
    #elif defined(LED_GPIO) && defined(LED_PIN)
    led_set(LED_GPIO, LED_PIN, BLINK_IS_ON(x), LED_IS_OPENDRAIN);
    #endif
    #ifdef USE_LED_STRIP
    WS2812_setRGB(BLINK_IS_RED(x) ? 255 : 0, BLINK_IS_GREEN(x) ? 255 : 0, BLINK_IS_BLUE(x) ? 255 : 0);
    #endif
    #ifdef ENABLE_LED_BLINK
    blink_span = BLINK_GET_TIME(x);
    #endif
}

void led_state_set(bool x)
{
    led_blink_set(x ? 0xE0 : 0);
}

void led_task(bool halt)
{
    #ifdef USE_LED_STRIP
    WS2812_task(); // this sets the next color if DMA is not busy
    #endif

    #ifdef ENABLE_LED_BLINK
    // only execute this every 100ms
    static uint32_t last_time = 0;
    if ((millis() - last_time) < 100) {
        return;
    }
    last_time = millis();

    blink_tmr++; // this will increment forever, 32 bits means 4971 days of operation without overflow

    uint8_t* active_pattern = (blink_secondary_pattern != NULL) ? blink_secondary_pattern : blink_primary_pattern;
    if (active_pattern == NULL) {
        return;
    }

    if ((blink_tmr - blink_last_time) >= blink_span) // time span has passed
    {
        blink_last_time = blink_tmr;
        blink_idx++; // advance to the next time slice in the pattern
        uint8_t x = active_pattern[blink_idx];
        if (x == BLINK_STOP)
        {
            if (halt) {
                return;
            }

            // loop around
            blink_idx = 0;
            if (blink_secondary_pattern != NULL) // we are in the secondary pattern, so stop the pattern and switch back to primary pattern
            {
                blink_secondary_pattern = NULL;
                active_pattern = blink_primary_pattern;
            }
            x = active_pattern[blink_idx];
        }
        led_blink_set(x);
    }
    #else
    #if !defined(USE_RGB_LED) && defined(LED_GPIO) && defined(LED_PIN)
    if (led_state == 0) {
        led_set(LED_GPIO, LED_PIN, false, LED_IS_OPENDRAIN);
    }
    else if (led_state == 1) {
        led_set(LED_GPIO, LED_PIN, (millis() % 1000) <= 100, LED_IS_OPENDRAIN);
    }
    else if (led_state == 2) {
        led_set(LED_GPIO, LED_PIN, (millis() % 500) <= 250, LED_IS_OPENDRAIN);
    }
    else if (led_state == 3) {
        led_set(LED_GPIO, LED_PIN, (millis() % 500) <= 400, LED_IS_OPENDRAIN);
    }
    #endif
    #endif
}

#ifdef ENABLE_LED_BLINK
void led_set_pattern_primary(const uint8_t* pattern)
{
    if (pattern != blink_primary_pattern) // prevent restarting the same pattern
    {
        blink_primary_pattern = (uint8_t*)pattern;

        // only start pattern if not interrupting the secondary pattern
        if (blink_secondary_pattern == NULL)
        {
            blink_idx  = 0;
            blink_span = 0;
            led_blink_set(pattern[blink_idx]);
        }
    }
}

void led_set_pattern_secondary(const uint8_t* pattern)
{
    if (pattern != blink_secondary_pattern) // prevent restarting the same pattern
    {
        blink_secondary_pattern = (uint8_t*)pattern;
        // start the pattern
        blink_idx  = 0;
        blink_span = 0;
        led_blink_set(pattern[blink_idx]);
    }
}

void ledblink_boot(void) {
    led_set_pattern_primary(blinkpattern_boot);
}

void ledblink_boot2(void) {
    led_set_pattern_primary(blinkpattern_boot2);
}

void ledblink_cli(void) {
    led_set_pattern_primary(blinkpattern_cli);
}

void ledblink_disarmed(void) {
    led_set_pattern_primary(blinkpattern_unarmed);
}

void ledblink_armed_stopped(void) {
    led_set_pattern_primary(blinkpattern_armed);
}

void ledblink_moving(void) {
    led_set_pattern_primary(blinkpattern_running);
}

#else

void ledblink_boot(void) {
    #ifdef USE_RGB_LED
    led_set(LED_GPIO_RED  , LED_PIN_RED  , false, LED_IS_OPENDRAIN);
    led_set(LED_GPIO_GREEN, LED_PIN_GREEN, false, LED_IS_OPENDRAIN);
    led_set(LED_GPIO_BLUE , LED_PIN_BLUE , true , LED_IS_OPENDRAIN);
    #elif defined(LED_GPIO) && defined(LED_PIN)
    led_state = 1;
    #endif
    #ifdef USE_LED_STRIP
    WS2812_setRGB(255, 0, 0);
    #endif
}

void ledblink_boot2(void) {
    #ifdef USE_RGB_LED
    led_set(LED_GPIO_RED  , LED_PIN_RED  , true , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_GREEN, LED_PIN_GREEN, false, LED_IS_OPENDRAIN);
    led_set(LED_GPIO_BLUE , LED_PIN_BLUE , true , LED_IS_OPENDRAIN);
    #elif defined(LED_GPIO) && defined(LED_PIN)
    led_state = 1;
    #endif
    #ifdef USE_LED_STRIP
    WS2812_setRGB(255, 0, 0);
    #endif
}

void ledblink_cli(void) {
    #ifdef USE_RGB_LED
    led_set(LED_GPIO_RED  , LED_PIN_RED  , true , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_GREEN, LED_PIN_GREEN, true , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_BLUE , LED_PIN_BLUE , true , LED_IS_OPENDRAIN);
    #elif defined(LED_GPIO) && defined(LED_PIN)
    led_state = 1;
    #endif
    #ifdef USE_LED_STRIP
    WS2812_setRGB(255, 0, 0);
    #endif
}

void ledblink_disarmed(void) {
    #ifdef USE_RGB_LED
    led_set(LED_GPIO_RED  , LED_PIN_RED  , true  , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_GREEN, LED_PIN_GREEN, false , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_BLUE , LED_PIN_BLUE , false , LED_IS_OPENDRAIN);
    #elif defined(LED_GPIO) && defined(LED_PIN)
    led_state = 1;
    #endif
    #ifdef USE_LED_STRIP
    WS2812_setRGB(255, 0, 0);
    #endif
}

void ledblink_armed_stopped(void) {
    #ifdef USE_RGB_LED
    led_set(LED_GPIO_RED  , LED_PIN_RED  , true  , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_GREEN, LED_PIN_GREEN, true  , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_BLUE , LED_PIN_BLUE , false , LED_IS_OPENDRAIN);
    #elif defined(LED_GPIO) && defined(LED_PIN)
    led_state = 2;
    #endif
    #ifdef USE_LED_STRIP
    WS2812_setRGB(255, 255, 0);
    #endif
}

void ledblink_moving(void) {
    #ifdef USE_RGB_LED
    led_set(LED_GPIO_RED  , LED_PIN_RED  , false , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_GREEN, LED_PIN_GREEN, true  , LED_IS_OPENDRAIN);
    led_set(LED_GPIO_BLUE , LED_PIN_BLUE , false , LED_IS_OPENDRAIN);
    #elif defined(LED_GPIO) && defined(LED_PIN)
    led_state = 3;
    #endif
    #ifdef USE_LED_STRIP
    WS2812_setRGB(0, 255, 0);
    #endif
}

#endif
