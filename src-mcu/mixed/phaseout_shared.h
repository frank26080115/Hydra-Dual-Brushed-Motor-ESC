// this file is meant to be included inline, do not include this file where it's not supposed to go

#include "phaseout_def_remap.h"

extern bool complementary_pwm;

#ifndef PWM_ENABLE_BRIDGE

#define PHASEOUTTEMPLATE_pwm_setPWM_x(X)    void pwm_setPWM_ ## X (void) {                            \
    if (!complementary_pwm) {                                                                         \
        LL_GPIO_SetPinMode(PHASE_GPIO_PORT_LOW_ ##X , PHASE_GPIO_LOW_ ##X , LL_GPIO_MODE_OUTPUT);     \
        PHASE_GPIO_PORT_LOW_ ##X ->LOW_BITREG_OFF = PHASE_GPIO_LOW_ ## X ;                            \
    }                                                                                                 \
    else {                                                                                            \
        LL_GPIO_SetPinMode(PHASE_GPIO_PORT_LOW_ ##X , PHASE_GPIO_LOW_ ##X , LL_GPIO_MODE_ALTERNATE);  \
    }                                                                                                 \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_HIGH_ ##X , PHASE_GPIO_HIGH_ ##X , LL_GPIO_MODE_ALTERNATE);    \
}                                                                                                     \

#define PHASEOUTTEMPLATE_pwm_setODPWM_x(X)    void pwm_setODPWM_ ## X (void) {                     \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_LOW_ ##X ,  PHASE_GPIO_LOW_ ##X ,  LL_GPIO_MODE_ALTERNATE); \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_HIGH_ ##X , PHASE_GPIO_HIGH_ ##X , LL_GPIO_MODE_OUTPUT);    \
    PHASE_GPIO_PORT_HIGH_ ##X ->LOW_BITREG_OFF =   PHASE_GPIO_HIGH_ ##X ;                          \
}                                                                                                  \

#define PHASEOUTTEMPLATE_pwm_setHIPWM_x(X)    void pwm_setHIPWM_ ## X (void) {                     \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_LOW_ ##X ,  PHASE_GPIO_LOW_ ##X ,  LL_GPIO_MODE_OUTPUT);    \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_HIGH_ ##X , PHASE_GPIO_HIGH_ ##X , LL_GPIO_MODE_ALTERNATE); \
    PHASE_GPIO_PORT_LOW_ ##X ->LOW_BITREG_OFF =   PHASE_GPIO_LOW_ ##X ;                            \
}                                                                                                  \

#define PHASEOUTTEMPLATE_pwm_setFlt_x(X)    void pwm_setFlt_ ##X () {                           \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_LOW_ ##X , PHASE_GPIO_LOW_ ##X , LL_GPIO_MODE_OUTPUT);   \
    PHASE_GPIO_PORT_LOW_ ##X ->LOW_BITREG_OFF = PHASE_GPIO_LOW_ ##X ;                           \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_HIGH_ ##X , PHASE_GPIO_HIGH_ ##X , LL_GPIO_MODE_OUTPUT); \
    PHASE_GPIO_PORT_HIGH_ ##X ->HIGH_BITREG_OFF = PHASE_GPIO_HIGH_ ##X ;                        \
}                                                                                               \

#define PHASEOUTTEMPLATE_pwm_setLow_x(X)    void pwm_setLow_ ##X () {                           \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_LOW_ ##X , PHASE_GPIO_LOW_ ##X , LL_GPIO_MODE_OUTPUT);   \
    PHASE_GPIO_PORT_LOW_ ##X ->LOW_BITREG_ON = PHASE_GPIO_LOW_ ##X ;                            \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_HIGH_ ##X , PHASE_GPIO_HIGH_ ##X , LL_GPIO_MODE_OUTPUT); \
    PHASE_GPIO_PORT_HIGH_ ##X ->HIGH_BITREG_OFF = PHASE_GPIO_HIGH_ ##X ;                        \
}                                                                                               \

#else

#define PHASEOUTTEMPLATE_pwm_setPWM_x(X)    void pwm_setPWM_ ## X (void) {                              \
    if (complementary_pwm) {                                                                            \
        LL_GPIO_SetPinMode(PHASE_GPIO_PORT_ENABLE_ ##X , PHASE_GPIO_ENABLE_ ##X , LL_GPIO_MODE_OUTPUT); \
        PHASE_GPIO_PORT_ENABLE_ ##X ->BSRR = PHASE_GPIO_ENABLE_ ##X ;                                   \
    }                                                                                                   \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_PWM_ ##X , PHASE_GPIO_PWM_ ##X , LL_GPIO_MODE_ALTERNATE);        \
}                                                                                                       \

#define PHASEOUTTEMPLATE_pwm_setFlt_x(X)    void pwm_setFlt_ ##X () {                               \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_ENABLE_ ##X , PHASE_GPIO_ENABLE_ ##X , LL_GPIO_MODE_OUTPUT); \
    PHASE_GPIO_PORT_ENABLE_ ##X ->LOW_BITREG_OFF = PHASE_GPIO_ENABLE_ ##X ;                         \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_PWM_ ##X , PHASE_GPIO_PWM_ ##X , LL_GPIO_MODE_OUTPUT);       \
    PHASE_GPIO_PORT_PWM_ ##X ->HIGH_BITREG_OFF = PHASE_GPIO_PWM_ ##X ;                              \
}                                                                                                   \

#define PHASEOUTTEMPLATE_pwm_setLow_x(X)    void pwm_setLow_ ##X () {                               \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_ENABLE_ ##X , PHASE_GPIO_ENABLE_ ##X , LL_GPIO_MODE_OUTPUT); \
    PHASE_GPIO_PORT_ENABLE_ ##X ->LOW_BITREG_ON = PHASE_GPIO_ENABLE_ ##X ;                          \
    LL_GPIO_SetPinMode(PHASE_GPIO_PORT_PWM_ ##X , PHASE_GPIO_PWM_ ##X , LL_GPIO_MODE_OUTPUT);       \
    PHASE_GPIO_PORT_PWM_ ##X ->HIGH_BITREG_OFF = PHASE_GPIO_PWM_ ##X ;                              \
}                                                                                                   \

// unable to do this, default to float for safety
#define PHASEOUTTEMPLATE_pwm_setODPWM_x(X)    void pwm_setODPWM_ ## X (void) {                      \
    pwm_setFlt_ ##X ();                                                                             \
}                                                                                                   \

// unable to do this, default to float for safety
#define PHASEOUTTEMPLATE_pwm_setHIPWM_x(X)    void pwm_setHIPWM_ ## X (void) {                      \
    pwm_setFlt_ ##X ();                                                                             \
}                                                                                                   \

#endif

PHASEOUTTEMPLATE_pwm_setPWM_x(A)
PHASEOUTTEMPLATE_pwm_setPWM_x(B)
PHASEOUTTEMPLATE_pwm_setPWM_x(C)
PHASEOUTTEMPLATE_pwm_setODPWM_x(A)
PHASEOUTTEMPLATE_pwm_setODPWM_x(B)
PHASEOUTTEMPLATE_pwm_setODPWM_x(C)
PHASEOUTTEMPLATE_pwm_setHIPWM_x(A)
PHASEOUTTEMPLATE_pwm_setHIPWM_x(B)
PHASEOUTTEMPLATE_pwm_setHIPWM_x(C)
PHASEOUTTEMPLATE_pwm_setFlt_x(A)
PHASEOUTTEMPLATE_pwm_setFlt_x(B)
PHASEOUTTEMPLATE_pwm_setFlt_x(C)
PHASEOUTTEMPLATE_pwm_setLow_x(A)
PHASEOUTTEMPLATE_pwm_setLow_x(B)
PHASEOUTTEMPLATE_pwm_setLow_x(C)
