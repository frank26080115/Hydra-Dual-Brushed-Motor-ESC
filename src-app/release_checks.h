#ifdef RELEASE_BUILD

#ifdef HW_TESTS
#error
#endif

#ifdef DEVELOPMENT_BOARD
#error
#endif

#if defined(DEBUG_PRINT) || defined(DEBUG_EVENTCNT) || defined(DEBUG_PINTOGGLE)
#error
#endif

#ifdef RC_LOG_JITTER
#error
#endif

#ifdef DISABLE_EEPROM
#error
#endif

#ifndef ENABLE_COMPILE_CLI
#error
#endif

#ifndef ENABLE_LED_BLINK
#error
#endif

#endif
