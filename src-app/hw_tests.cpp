#include "main.h"
#include "hw_tests.h"
#include "debug_tools.h"
#include "phaseout.h"
#include "sense.h"
#include "led.h"
#include "crsf.h"
#include "userconfig.h"
#include "swd_pins.h"

#ifdef DEVELOPMENT_BOARD
extern Cereal_USART dbg_cer;
#endif

extern RcPulse_InputCap rc_pulse_1;
extern RcPulse_GpioIsr  rc_pulse_2;
extern CrsfChannel      crsf_1;
extern CrsfChannel      crsf_2;
extern Cereal_USART     main_cer;

extern RcChannel* rc1;
extern RcChannel* rc2;

#ifdef ENABLE_COMPILE_CLI
extern Cereal_TimerBitbang cli_cer;
#endif

extern void cli_enter(void);

#ifdef HW_TESTS
void hw_test(void)
{
    //hwtest_uart();
    //hwtest_uart1();
    //hwtest_uart2();
    //hwtest_uart_swc();
    //hwtest_adc();
    //hwtest_sense();
    //hwtest_gpio(GPIOA, LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10);
    //hwtest_led();
    //hwtest_pwm();
    hwtest_pwm_max();
    //hwtest_phases();
    //hwtest_rc1();
    //hwtest_rc2();
    //hwtest_rc12();
    //hwtest_rc_crsf();
    //hwtest_bbcer();
    //hwtest_eeprom();
    //hwtest_cli();
    //hwtest_tone();
    //hwtest_rc_led_crsf();
    //hwtest_rc_tone_crsf();
}
#endif

void hwtest_gpio(
    #if defined(STMICRO)
        GPIO_TypeDef
    #elif defined(ARTERY)
        gpio_type
    #endif
        * gpio, uint32_t pin)
{
    #if defined(STMICRO)
    LL_GPIO_InitTypeDef GPIO_InitStruct    = {0};

    GPIO_InitStruct.Pin        = pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init(gpio, &GPIO_InitStruct);
    #elif defined(ARTERY)
    gpio_mode_QUICK(gpio, GPIO_MODE_OUTPUT, GPIO_PULL_NONE, pin);
    #endif

    while (true)
    {
        uint32_t t = millis();
        t %= 2000;
        if (t < 1000) {
            #if defined(STMICRO)
                LL_GPIO_SetOutputPin
            #elif defined(ARTERY)
                gpio_bits_set
            #endif
                (gpio, pin);
        }
        else {
            #if defined(STMICRO)
                LL_GPIO_ResetOutputPin
            #elif defined(ARTERY)
                gpio_bits_reset
            #endif
                (gpio, pin);
        }
    }
}

void hwtest_led(void)
{
    led_init();
    ledblink_cliplug();
    while (true)
    {
        led_task(false);
    }
}

void hwtest_pwm(void)
{
    dbg_button_init();
    pwm_init();
    dbg_switch_to_pwm();
    pwm_all_flt();
    pwm_set_braking(true);
    pwm_all_pwm();
    pwm_set_remap(1);
    pwm_set_loadbalance(false);
    while (true)
    {
        #ifdef DEVELOPMENT_BOARD
        if (dbg_read_btn())
        {
            uint32_t x = (millis() / 5) % (PWM_DEFAULT_PERIOD);
            pwm_set_all_duty_remapped(x, x + 1, x + 2);
        }
        else
        {
            pwm_set_all_duty_remapped(0, 0, 0);
        }
        #else
        uint32_t tnow = millis();
        uint32_t tnow10 = tnow % 20000;
        if (tnow10 < 3000)
        {
            pwm_set_all_duty_remapped(0, 0, 0);
        }
        else
        {
            uint32_t lim = PWM_DEFAULT_PERIOD;
            uint32_t x = tnow10;
            pwm_set_all_duty_remapped(x % lim, (x + (lim / 3)) % lim, (x + ((lim * 2) / 3)) % lim);
        }
        #endif
    }
}

void hwtest_pwm_max(void)
{
    dbg_button_init();
    pwm_init();
    dbg_switch_to_pwm();
    pwm_all_flt();
    pwm_set_braking(true);
    pwm_all_pwm();
    pwm_set_period(PWM_DEFAULT_PERIOD);
    pwm_set_deadtime(PWM_DEFAULT_DEADTIME);
    pwm_set_remap(1);
    pwm_set_loadbalance(false);
    while (true)
    {
        #ifdef DEVELOPMENT_BOARD
        if (dbg_read_btn())
        {
            pwm_set_all_duty_remapped(PWM_DEFAULT_PERIOD, PWM_DEFAULT_PERIOD - PWM_DEFAULT_DEADTIME, PWM_DEFAULT_PERIOD - PWM_DEFAULT_DEADTIME - PWM_DEFAULT_DEADTIME);
        }
        else
        {
            pwm_set_all_duty_remapped(0, 0, 0);
        }
        #else
        uint32_t tnow = millis();
        uint32_t tnow10 = tnow % 20000;
        if (tnow10 < 3000)
        {
            pwm_set_all_duty_remapped(0, 0, 0);
        }
        else
        {
            pwm_set_all_duty_remapped(PWM_DEFAULT_PERIOD, PWM_DEFAULT_PERIOD - PWM_DEFAULT_DEADTIME, PWM_DEFAULT_PERIOD - PWM_DEFAULT_DEADTIME - PWM_DEFAULT_DEADTIME);
        }
        #endif
    }
}

void hwtest_adc(void)
{
    Cereal* cer;
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    cer = &dbg_cer;
    #elif defined(ENABLE_COMPILE_CLI)
    #if defined(MAIN_SIGNAL_PA2)
    main_cer.init(CEREAL_ID_USART2, CLI_BAUD, false, false);
    cer = &main_cer;
    #else
    cli_cer.init(CLI_BAUD);
    cer = &cli_cer;
    #endif
    #endif

    eeprom_load_defaults();
    sense_init();
    uint32_t t = millis();
    while (true)
    {
        sense_task();
        if ((millis() - t) >= 200)
        {
            t = millis();
            cer->printf("[%lu]  v = %u / %lu    c = %u / %lu    t = %u / %lu\r\n", t
                , adc_raw_voltage, sense_voltage
                , adc_raw_current, sense_current
                , adc_raw_temperature, sense_temperatureC
                );
        }
    }
}

void hwtest_sense(void)
{
    Cereal* cer;
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    cer = &dbg_cer;
    #elif defined(ENABLE_COMPILE_CLI)
    #if defined(MAIN_SIGNAL_PA2)
    main_cer.init(CEREAL_ID_USART2, CLI_BAUD, false, false);
    cer = &main_cer;
    #else
    cli_cer.init(CLI_BAUD);
    cer = &cli_cer;
    #endif
    #endif

    eeprom_load_defaults();
    sense_init();
    uint32_t t = millis();
    while (true)
    {
        sense_task();
        if ((millis() - t) >= 200)
        {
            t = millis();
            cer->printf("[%lu]  v = %lu     c = %lu    t = %lu\r\n", t, sense_voltage, sense_current, sense_temperatureC);
        }
    }
}

void hwtest_uart(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    uint32_t t = millis();
    while (true)
    {
        if ((millis() - t) >= 1000) {
            t = millis();
            dbg_cer.printf("[%lu] test\r\n", t);
        }
        if (dbg_cer.available() > 0)
        {
            dbg_cer.write((uint8_t)dbg_cer.read());
        }
    }
    #endif
}

void hwtest_uart1(void)
{
    main_cer.init(CEREAL_ID_USART1, CLI_BAUD, true, false);
    uint32_t t = millis();
    while (millis() < 10000)
    {
        if ((millis() - t) >= 1000) {
            t = millis();
            main_cer.printf("[%lu] test\r\n", t);
        }
        if (main_cer.available() > 0)
        {
            main_cer.write((uint8_t)main_cer.read());
        }
    }
    NVIC_SystemReset();
}

void hwtest_uart2(void)
{
    main_cer.init(CEREAL_ID_USART2, CLI_BAUD, true, false);
    uint32_t t = millis();
    while (millis() < 10000)
    {
        if ((millis() - t) >= 1000) {
            t = millis();
            main_cer.printf("[%lu] test\r\n", t);
        }
        if (main_cer.available() > 0)
        {
            main_cer.write((uint8_t)main_cer.read());
        }
    }
    NVIC_SystemReset();
}

void hwtest_uart_swc(void)
{
    main_cer.init(CEREAL_ID_USART_SWCLK, CLI_BAUD, true, false);
    uint32_t t = millis();
    while (millis() < 10000)
    {
        if ((millis() - t) >= 1000) {
            t = millis();
            main_cer.printf("[%lu] test\r\n", t);
        }
        if (main_cer.available() > 0)
        {
            main_cer.write((uint8_t)main_cer.read());
        }
    }
    NVIC_SystemReset();
}

void hwtest_rcx(RcChannel* rcx, RcChannel* rcx2, uint8_t idx);
void hwtest_rcx_print(RcChannel* rcx, uint8_t idx);
void hwtest_rc_led(RcChannel* rcx, RcChannel* rcx2);

void hwtest_rc1(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    #endif
    eeprom_load_defaults();
    load_runtime_configs();
    rc_pulse_1.init();
    hwtest_rcx(&rc_pulse_1, NULL, 1);
}

void hwtest_rc2(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    #endif
    eeprom_load_defaults();
    load_runtime_configs();
    rc_pulse_2.init(GPIOEXTI_TIMx, GPIOEXTI_GPIO, GPIOEXTI_Pin);
    hwtest_rcx(&rc_pulse_2, NULL, 2);
}

void hwtest_rc12(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    #endif
    eeprom_load_defaults();
    load_runtime_configs();
    rc_pulse_1.init();
    rc_pulse_2.init(GPIOEXTI_TIMx, GPIOEXTI_GPIO, GPIOEXTI_Pin);
    hwtest_rcx(&rc_pulse_1, &rc_pulse_2, 1);
}

void hwtest_rc_crsf(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    #endif
    eeprom_load_defaults();
    load_runtime_configs();
    main_cer.init(CEREAL_ID_USART_CRSF, 420000, true, false, true);
    crsf_1.init(&main_cer, 1);
    crsf_2.init(&main_cer, 2);
    hwtest_rcx(&crsf_1, &crsf_2, 1);
}

void hwtest_rcx_print(RcChannel* rcx, uint8_t idx)
{
    dbg_printf("%c%c%u = %d"
        #ifdef RC_LOG_JITTER
        "  (j %u)"
        #endif
        , rcx->is_alive() ? 'R' : 'r'
        , rcx->is_armed() ? 'C' : 'c'
        , idx
        , rcx->readRaw()
        #ifdef RC_LOG_JITTER
        , rcx->readJitter()
        #endif
            );
}

void hwtest_rcx(RcChannel* rcx, RcChannel* rcx2, uint8_t idx)
{
    uint32_t t = millis();
    while (true)
    {
        rcx->task();
        if (rcx2 != NULL) {
            rcx2->task();
        }
        if ((millis() - t) >= 200)
        {
            t = millis();
            dbg_printf("[%u] ", millis());
            hwtest_rcx_print(rcx, idx);
            if (rcx2 != NULL) {
                dbg_printf("  ;  ");
                hwtest_rcx_print(rcx2, idx + 1);
            }
            dbg_printf("\r\n");
        }
    }
}

void hwtest_rc_led_pwm()
{
    eeprom_load_defaults();
    load_runtime_configs();
    rc_pulse_1.init();
    rc_pulse_2.init(GPIOEXTI_TIMx, GPIOEXTI_GPIO, GPIOEXTI_Pin);
    hwtest_rc_led(&rc_pulse_1, &rc_pulse_2);
}

void hwtest_rc_led_crsf()
{
    eeprom_load_defaults();
    load_runtime_configs();
    #if defined(MAIN_SIGNAL_PA2)
    crsf_inputGuess = 2;
    #endif
    main_cer.init(CEREAL_ID_USART_CRSF, 420000, true, false, true);
    crsf_1.init(&main_cer, 1);
    crsf_2.init(&main_cer, 2);
    hwtest_rc_led(&crsf_1, &crsf_2);
}

void hwtest_rc_led(RcChannel* rcx, RcChannel* rcx2)
{
    led_init();
    while (true)
    {
        rcx->task();
        if (rcx2 != NULL) {
            rcx2->task();
        }
        bool alive = false;
        int mv = 0;
        int r;
        if (rcx->is_alive()) {
            alive |= true;
            r = rcx->read();
            r = r < 0 ? -r : r;
            mv = r > mv ? r : mv;
        }
        if (rcx2 != NULL)
        {
            if (rcx2->is_alive()) {
                alive |= true;
                r = rcx2->read();
                r = r < 0 ? -r : r;
                mv = r > mv ? r : mv;
            }
        }
        if (alive == false)
        {
            if ((millis() % 1000) <= 100)
            {
                led_blink_set(1 << 5);
            }
            else
            {
                led_blink_set(0);
            }
        }
        else
        {
            uint32_t tlim = fi_map(mv, 0, THROTTLE_UNIT_RANGE, 20, 100, true);
            if ((millis() % 100) <= tlim) {
                led_blink_set(1 << 7);
            }
            else {
                led_blink_set(0);
            }
        }
    }
}

#if defined(DISABLE_LED) || defined(ENABLE_TONE)

void hwtest_rc_tone(RcChannel* rcx, RcChannel* rcx2);

void hwtest_rc_tone_pwm()
{
    eeprom_load_defaults();
    load_runtime_configs();
    pwm_init();
    dbg_switch_to_pwm();
    rc_pulse_1.init();
    rc_pulse_2.init(GPIOEXTI_TIMx, GPIOEXTI_GPIO, GPIOEXTI_Pin);
    hwtest_rc_tone(&rc_pulse_1, &rc_pulse_2);
}

void hwtest_rc_tone_crsf()
{
    eeprom_load_defaults();
    load_runtime_configs();
    pwm_init();
    dbg_switch_to_pwm();
    #if defined(MAIN_SIGNAL_PA2)
    crsf_inputGuess = 2;
    #endif
    main_cer.init(CEREAL_ID_USART_CRSF, 420000, true, false, true);
    crsf_1.init(&main_cer, 1);
    crsf_2.init(&main_cer, 2);
    hwtest_rc_tone(&crsf_1, &crsf_2);
}

void hwtest_rc_tone(RcChannel* rcx, RcChannel* rcx2)
{
    tone_start(2, 0, 100);
    while (true)
    {
        rcx->task();
        if (rcx2 != NULL) {
            rcx2->task();
        }
        bool alive = false;
        int mv = 0;
        int r;
        if (rcx->is_alive()) {
            alive |= true;
            r = rcx->read();
            r = r < 0 ? -r : r;
            mv = r > mv ? r : mv;
        }
        if (rcx2 != NULL)
        {
            if (rcx2->is_alive()) {
                alive |= true;
                r = rcx2->read();
                r = r < 0 ? -r : r;
                mv = r > mv ? r : mv;
            }
        }
        if (alive == false)
        {
            if ((millis() % 1000) <= 100)
            {
                tone_setVolume(50);
            }
            else
            {
                tone_setVolume(0);
            }
        }
        else
        {
            uint32_t x = fi_map(mv, 0, THROTTLE_UNIT_RANGE, 10, 100, true);
            tone_setVolume(x);
        }
    }
}

#endif

#ifdef ENABLE_COMPILE_CLI
void hwtest_bbcer(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    #endif
    cli_cer.init(CLI_BAUD);
    uint32_t t = millis();
    while (true)
    {
        if ((millis() - t) >= 200)
        {
            t = millis();
            cli_cer.printf("\r\n> %lu\r\n", t);
            dbg_printf("\r\n< %lu\r\n", t);
        }
        #ifdef DEVELOPMENT_BOARD
        int16_t c;
        c = dbg_cer.read();
        if (c >= 0) {
            cli_cer.write((uint8_t)c);
        }
        c = cli_cer.read();
        if (c >= 0) {
            dbg_cer.write((uint8_t)c);
        }
        #endif
    }
}
#endif

void hwtest_eeprom(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    dbg_button_init();
    uint32_t t = millis();
    uint32_t i, j;
    while (true)
    {
        if (dbg_read_btn())
        {
            dbg_printf("EEPROM addr 0x%08X\r\n", cfg_addr);
            cfg.useless = 0x12345678;
            eeprom_factory_reset();
            dbg_printf("checksum after factory reset: 0x%02X\r\n", cfg.chksum);
            dbg_printf("dump after factory reset:");
            for (i = 0, j = cfg_addr; i < sizeof(EEPROM_data_t); i++, j++)
            {
                if ((i % 16) == 0) {
                    dbg_printf("\r\n");
                }
                uint8_t* bptr = (uint8_t*)j;
                dbg_printf("%02X ", *bptr);
            }
            dbg_printf("\r\n");
            cfg.useless = 0xDEADBEEF;
            eeprom_save();
            dbg_printf("checksum after save: 0x%02X\r\n", cfg.chksum);
            dbg_printf("test pattern after save: 0x%08X\r\n", cfg.useless);
            dbg_printf("dump after save:");
            for (i = 0, j = cfg_addr; i < sizeof(EEPROM_data_t); i++, j++)
            {
                if ((i % 16) == 0) {
                    dbg_printf("\r\n");
                }
                uint8_t* bptr = (uint8_t*)j;
                dbg_printf("%02X ", *bptr);
            }
            dbg_printf("\r\n");
            cfg.useless = 0; // load should restore this back to 0xDEADBEEF
            eeprom_load_or_default();
            dbg_printf("checksum after load: 0x%02X\r\n", cfg.chksum);
            dbg_printf("test pattern after load: 0x%08X\r\n", cfg.useless);
            while (true) {
                // do nothing
            }
        }
        else {
            if ((millis() - t) >= 1000) {
                t = millis();
                dbg_printf("[%u] press button to test EEPROM\r\n", t);
            }
        }
    }
    #endif
}

void hwtest_cli(void)
{
    led_init();
    eeprom_load_defaults();
    sense_init();
    pwm_init();
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false);
    rc_pulse_1.init();
    rc_pulse_2.init(GPIOEXTI_TIMx, GPIOEXTI_GPIO, GPIOEXTI_Pin);
    rc1 = &rc_pulse_1;
    rc2 = &rc_pulse_2;
    #endif
    cli_enter();
}

void hwtest_phases(void)
{
    eeprom_load_defaults();
    load_runtime_configs();
    dbg_button_init();
    pwm_init();
    dbg_switch_to_pwm();
    pwm_set_braking(true);
    pwm_set_loadbalance(false);
    while (true)
    {
        uint32_t x = PWM_DEFAULT_PERIOD;
        pwm_set_all_duty_remapped(x, x / 2, x / 4);
    }
}

void hwtest_tone(void)
{
    eeprom_load_defaults();
    load_runtime_configs();
    dbg_button_init();
    pwm_init();
    dbg_switch_to_pwm();
    uint32_t t = 0;
    uint8_t i;
    while (true)
    {
        uint32_t now = millis();
        tone_task();
        #ifdef DEVELOPMENT_BOARD
        if (dbg_read_btn())
        {
            if ((now - t) >= 1000) {
                i++;
                tone_start(i, 0, 100);
                t = now;
            }
        }
        else
        {
            tone_stop();
            i = 0;
        }
        #else
        uint32_t tnow10 = now % 10000;
        if (tnow10 < 3000)
        {
            tone_stop();
            i = 0;
        }
        else
        {
            if ((now - t) >= 1000) {
                i++;
                tone_start(i, 0, 30);
                t = now;
            }
        }
        #endif
    }
}
