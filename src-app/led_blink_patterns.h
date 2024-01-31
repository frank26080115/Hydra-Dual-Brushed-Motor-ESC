#pragma once

#include <stdint.h>
#include <stdbool.h>

#define BLINK_TIME_MASK               0x1F
#define BLINK_STOP                    0
#define BLINK_LOOP                    0 // same as stop
#define BLINK_RGB_TIME(r, g, b, t)    ((((r) ? 1 : 0) << 5) | (((g) ? 1 : 0) << 6) | (((b) ? 1 : 0) << 7) | ((t) & BLINK_TIME_MASK))
#define BLINK_ON_TIME(t)              BLINK_RGB_TIME(1, 0, 0, t) // borrow above macro
#define BLINK_OFF_TIME(t)             BLINK_RGB_TIME(0, 0, 0, t)

#define BLINK_GET_TIME(x)             ((x) & BLINK_TIME_MASK)
#define BLINK_IS_RED(x)               (((x) & (1 << 5)) != 0)
#define BLINK_IS_GREEN(x)             (((x) & (1 << 6)) != 0)
#define BLINK_IS_BLUE(x)              (((x) & (1 << 7)) != 0)
#define BLINK_IS_OFF(x)               (((x) & 0xE0) == 0)
#define BLINK_IS_ON(x)                (!BLINK_IS_OFF(x))

// patterns are defined here
// each byte is a frame
// the top 3 bits indicate which of the RGB LEDs is on or off_type
// the bottom 5 bits is the time span of the frame
// time span is in units of 100ms, if 5 bits is not enough, then simply duplicate the frame
// each pattern must end with a 0 byte, and will be looped or stopped on that frame

#if defined(USE_RGB_LED) || defined(USE_LED_STRIP)

const uint8_t blinkpattern_cliwait[] = {
    // like disarmed but with a pause
    BLINK_RGB_TIME(1, 0, 0, 8),
    BLINK_OFF_TIME(1),
    BLINK_LOOP,
};

const uint8_t blinkpattern_cliplug[] = {
    // rapid multi-coloured blink
    BLINK_RGB_TIME(1, 0, 0, 1),
    BLINK_OFF_TIME(1),
    BLINK_RGB_TIME(0, 1, 0, 1),
    BLINK_OFF_TIME(1),
    BLINK_RGB_TIME(0, 0, 1, 1),
    BLINK_OFF_TIME(1),
    BLINK_LOOP,
};

const uint8_t blinkpattern_cli[] = {
    // fast double purple blink
    BLINK_RGB_TIME(1, 0, 1, 1),
    BLINK_OFF_TIME(1),
    BLINK_RGB_TIME(1, 0, 1, 1),
    BLINK_OFF_TIME(1),
    BLINK_LOOP,
};

const uint8_t blinkpattern_unarmed[] = {
    // unarmed should just make red LED stay on always
    BLINK_RGB_TIME(1, 0, 0, 1),
    BLINK_LOOP,
};

const uint8_t blinkpattern_armed[] = {
    BLINK_RGB_TIME(0, 1, 0, 1),
    BLINK_OFF_TIME(4),
    BLINK_LOOP,
};

const uint8_t blinkpattern_running[] = {
    BLINK_RGB_TIME(0, 1, 0, 2),
    BLINK_OFF_TIME(2),
    BLINK_RGB_TIME(0, 1, 0, 2),
    BLINK_OFF_TIME(2),
    BLINK_LOOP,
};

const uint8_t blinkpattern_currentlimited[] = {
    BLINK_RGB_TIME(1, 0, 0, 3),
    BLINK_OFF_TIME(1),
    BLINK_RGB_TIME(1, 0, 1, 1),
    BLINK_OFF_TIME(1),
    BLINK_RGB_TIME(1, 0, 0, 3),
    BLINK_OFF_TIME(1),
    BLINK_RGB_TIME(1, 0, 1, 1),
    BLINK_OFF_TIME(1),
    BLINK_STOP,
};

const uint8_t blinkpattern_lowbatt[] = {
    BLINK_RGB_TIME(1, 0, 0, 2),
    BLINK_OFF_TIME(5),
    BLINK_RGB_TIME(0, 0, 1, 2),
    BLINK_OFF_TIME(5),
    BLINK_STOP,
};

#else

const uint8_t blinkpattern_cliwait[] = {
    BLINK_ON_TIME(8),
    BLINK_OFF_TIME(1),
    BLINK_LOOP,
};

const uint8_t blinkpattern_cliplug[] = {
    BLINK_ON_TIME(5),
    BLINK_OFF_TIME(1),
    BLINK_ON_TIME(2),
    BLINK_OFF_TIME(1),
    BLINK_LOOP,
};

const uint8_t blinkpattern_cli[] = {
    BLINK_ON_TIME(10),
    BLINK_OFF_TIME(10),
    BLINK_LOOP,
};

const uint8_t blinkpattern_unarmed[] = {
    // unarmed should just make red LED stay on always
    BLINK_ON_TIME(5),
    BLINK_LOOP,
};

const uint8_t blinkpattern_armed[] = {
    // slow double blink
    BLINK_ON_TIME(1),
    BLINK_OFF_TIME(1),
    BLINK_ON_TIME(1),
    BLINK_OFF_TIME(7),
    BLINK_LOOP,
};

const uint8_t blinkpattern_running[] = {
    // fast blink
    BLINK_ON_TIME(1),
    BLINK_OFF_TIME(1),
    BLINK_ON_TIME(1),
    BLINK_OFF_TIME(1),
    BLINK_LOOP,
};

const uint8_t blinkpattern_currentlimited[] = {
    BLINK_ON_TIME(3),
    BLINK_OFF_TIME(1),
    BLINK_ON_TIME(1),
    BLINK_OFF_TIME(1),
    BLINK_STOP,
};

const uint8_t blinkpattern_lowbatt[] = {
    BLINK_ON_TIME(3),
    BLINK_OFF_TIME(5),
    BLINK_ON_TIME(1),
    BLINK_OFF_TIME(5),
    BLINK_STOP,
};

#endif
