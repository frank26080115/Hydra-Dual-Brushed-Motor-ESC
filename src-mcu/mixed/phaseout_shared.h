// this file is meant to be included inline, do not include this file where it's not supposed to go

#include "phaseout_def_remap.h"

extern bool complementary_pwm;

void pwm_proportional_brake()
{
    LL_GPIO_SetPinMode(PHASE_A_GPIO_PORT_HIGH, PHASE_A_GPIO_HIGH, LL_GPIO_MODE_OUTPUT);
    PHASE_A_GPIO_PORT_HIGH->HIGH_BITREG_OFF = PHASE_A_GPIO_HIGH;

    LL_GPIO_SetPinMode(PHASE_B_GPIO_PORT_HIGH, PHASE_B_GPIO_HIGH, LL_GPIO_MODE_OUTPUT);
    PHASE_B_GPIO_PORT_HIGH->HIGH_BITREG_OFF = PHASE_B_GPIO_HIGH;

    LL_GPIO_SetPinMode(PHASE_C_GPIO_PORT_HIGH, PHASE_C_GPIO_HIGH, LL_GPIO_MODE_OUTPUT);
    PHASE_C_GPIO_PORT_HIGH->HIGH_BITREG_OFF = PHASE_C_GPIO_HIGH;

    // set low channel to PWM, duty cycle will now control braking
    LL_GPIO_SetPinMode(PHASE_A_GPIO_PORT_LOW, PHASE_A_GPIO_LOW, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(PHASE_B_GPIO_PORT_LOW, PHASE_B_GPIO_LOW, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(PHASE_C_GPIO_PORT_LOW, PHASE_C_GPIO_LOW, LL_GPIO_MODE_ALTERNATE);
}

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

// unable to do this
#define PHASEOUTTEMPLATE_pwm_setODPWM_x(X)    void pwm_setODPWM_ ## X (void) {                      \
}                                                                                                   \

// unable to do this
#define PHASEOUTTEMPLATE_pwm_setHIPWM_x(X)    void pwm_setHIPWM_ ## X (void) {                      \
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
