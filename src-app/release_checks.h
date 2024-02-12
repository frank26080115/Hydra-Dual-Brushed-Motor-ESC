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

#ifdef SIMULATE_CURRENT_LIMIT
#error
#endif
#ifdef SIMULATE_VOLTAGE_LIMIT
#error
#endif
#ifdef SIMULATE_TEMPERATURE_LIMIT
#error
#endif

#endif
