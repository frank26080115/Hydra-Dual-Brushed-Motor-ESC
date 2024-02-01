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
    //hwtest_adc();
    //hwtest_sense();
    //hwtest_gpio(GPIOA, LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10);
    //hwtest_led();
    //hwtest_pwm();
    //hwtest_rc1();
    //hwtest_rc2();
    hwtest_rc_crsf();
    //hwtest_bbcer();
    //hwtest_eeprom();
    //hwtest_cli();
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
    pwm_set_reload(2000);
    pwm_set_remap(1);
    pwm_set_loadbalance(false);
    while (true)
    {
        if (dbg_read_btn())
        {
            uint32_t x = (millis() / 5) % 1900;
            pwm_set_all_duty_remapped(x, x + 1, x + 2);
        }
        else
        {
            pwm_set_all_duty_remapped(0, 0, 0);
        }
    }
}

void hwtest_adc(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
    #endif
    sense_init();
    uint32_t t = millis();
    while (true)
    {
        sense_task();
        if ((millis() - t) >= 200)
        {
            t = millis();
            dbg_printf("[%u]  v = %u     c = %u    t = %u\r\n", t, adc_raw_voltage, adc_raw_current, adc_raw_temperature);
        }
    }
}

void hwtest_sense(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
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
            dbg_printf("[%u]  v = %u     c = %u    t = %u\r\n", t, sense_voltage, sense_current, sense_temperatureC);
        }
    }
}

void hwtest_uart(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
    while (true)
    {
        //if (dbg_cer.get_idle_flag(false))
        //{
        //    if (dbg_cer.available() > 0)
        //    {
        //        int len = dbg_cer.available();
        //        uint8_t* buf = dbg_cer.get_buffer();
        //        dbg_cer.writeb(buf, len);
        //        dbg_cer.flush();
        //        dbg_cer.reset_buffer();
        //        dbg_cer.get_idle_flag(true);
        //    }
        //}
        if (dbg_cer.available() > 0)
        {
            dbg_cer.write((uint8_t)dbg_cer.read());
        }
    }
    #endif
}

void hwtest_rcx(RcChannel* rcx, uint8_t idx);
void hwtest_rcx_print(RcChannel* rcx, uint8_t idx);

void hwtest_rc1(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
    #endif
    rc_pulse_1.init();
    hwtest_rcx(&rc_pulse_1, 1);
}

void hwtest_rc2(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
    #endif
    rc_pulse_2.init(GPIOEXTI_TIMx, GPIOEXTI_GPIO, GPIOEXTI_Pin);
    hwtest_rcx(&rc_pulse_2, 2);
}

void hwtest_rc_crsf(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
    #endif
    eeprom_load_defaults();
    load_runtime_configs();
    main_cer.init(CEREAL_ID_USART_CRSF, 420000, false, true, false, true);
    crsf_1.init(&main_cer, 1);
    crsf_2.init(&main_cer, 2);
    uint32_t t = millis();
    while (true)
    {
        crsf_1.task();
        crsf_2.task();
        if ((millis() - t) >= 200)
        {
            t = millis();
            dbg_printf("[%u] ", millis());
            hwtest_rcx_print(&crsf_1, 1);
            dbg_printf("  ;  ");
            hwtest_rcx_print(&crsf_2, 2);
            dbg_printf("\r\n");
        }
    }
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

void hwtest_rcx(RcChannel* rcx, uint8_t idx)
{
    uint32_t t = millis();
    while (true)
    {
        rcx->task();
        if ((millis() - t) >= 200)
        {
            t = millis();
            dbg_printf("[%u] ", millis());
            hwtest_rcx_print(rcx, idx);
            dbg_printf("\r\n");
        }
    }
}

#ifdef ENABLE_COMPILE_CLI
void hwtest_bbcer(void)
{
    #ifdef DEVELOPMENT_BOARD
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
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
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
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
    dbg_cer.init(CEREAL_ID_USART_DEBUG, DEBUG_BAUD, false, false, false);
    rc_pulse_1.init();
    rc_pulse_2.init(GPIOEXTI_TIMx, GPIOEXTI_GPIO, GPIOEXTI_Pin);
    rc1 = &rc_pulse_1;
    rc2 = &rc_pulse_2;
    #endif
    cli_enter();
}
