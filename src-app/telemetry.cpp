#include "main.h"
#include "telemetry.h"
#include "crsf.h"
#include "sense.h"
#include "userconfig.h"
#include "stm32_at32_compat.h"

#ifdef ENABLE_TELEMETRY

// bit flags that indicate which pin to initialize and how
// the names indicate what the pin originally was used for, and what they are going to be used for
enum
{
    TELEMPIN_NO_NEED,
    TELEMPIN_PA2_WAS_CRSF   = 0x01,
    TELEMPIN_PA2_WAS_PWM    = 0x02,
    TELEMPIN_PB4_WAS_PWM    = 0, // do nothing
    TELEMPIN_PB6_WAS_CRSF   = 0x04,
    TELEMPIN_PB6_WAS_PWM    = 0x08,
    TELEMPIN_PA14_WAS_CRSF  = 0x10,
    TELEMPIN_PA14_WAS_PWM   = 0x20,
    TELEMPIN_PB6_TO_TELEM   = 0x40,
    TELEMPIN_PA14_TO_TELEM  = 0x80,
};

extern "C"
{
    // telemetry needs to read from the sense module
    extern uint32_t sense_current;
    extern uint32_t sense_voltage;
    extern uint8_t  batt_cell_cnt;
    extern uint32_t batt_starting_voltage;
    extern uint32_t sense_current_accum;
    extern uint32_t sense_current_accum_cnt;
}

extern Cereal_USART  main_cer;         // from main
static Cereal_USART* telem_cer = NULL; // initialized only if required
static uint8_t  telem_swapmode;        // caches simplified byte indicating which pin needs to be initialized and how
static uint32_t telem_period;          // milliseconds between telemetry packets
static uint8_t  telem_datamask = 0;    // what data is reported in the telemetry packet, using the `TELEMPORT_FLAG_DISABLE_*` enumerations
static bool     telem_callFromCrsf = false; // will be set if the CRSF reception calls the task, and subsequent telemetry can only be sent right after CRSF reception
static uint32_t telem_lastTime = 0;    // timestamp of the last telemetry packet
static uint32_t telem_mAs = 0;         // tracks milliamp-seconds, in order to report milliamp-hours
static uint32_t telem_mAH_max = 0;     // mAH cannot go down due to accumulator rounding errors
static uint32_t telem_mAH_time = 0;    // used to track second ticks
static uint32_t telem_mAH_maxCnt = 0;  // used to average out the accumlated milliamp readings over one second

extern uint8_t crsf_crc8(const uint8_t *ptr, uint8_t len);
static void telem_pinPrep(void);
static void telem_pinRestore(void);
static void telem_init(Cereal_USART* cer, uint8_t swap_mode);

#define TELEM_BIGENDIAN16(_buf, _idx, _val)    do {(_buf)[_idx] = (_val) >> 8;  (_buf)[(_idx) + 1] = (_val) & 0xFF;} while (0)
#define TELEM_BIGENDIAN24(_buf, _idx, _val)    do {(_buf)[_idx] = (_val) >> 16; (_buf)[(_idx) + 1] = ((_val) >> 8) & 0xFF; (_buf)[(_idx) + 2] = (_val) & 0xFF;} while (0)

void telemetry_init(void)
{
    if (cfg.telemetry_rate <= 0 || cfg.telemetry_port == TELEMPORT_NONE) {
        return;
    }

    uint8_t telemetry_port = cfg.telemetry_port & 0x0F; // removes the flags
    uint8_t telem_pin = telemetry_port == TELEMPORT_PB6 ? TELEMPIN_PB6_TO_TELEM : TELEMPIN_PA14_TO_TELEM; 
    if (telemetry_port == TELEMPORT_PB6) // a whole new USART needs to be initialized
    {
        #if defined(MAIN_SIGNAL_PA2)
        telem_cer = new Cereal_USART();
        telem_cer->init(CEREAL_ID_USART1, (cfg.telemetry_baud == 0) ? CRSF_BAUDRATE : cfg.telemetry_baud, false, false);
        #elif defined(MAIN_SIGNAL_PB4)
        telem_cer = &main_cer;
        #endif
        if (cfg.input_mode == INPUTMODE_CRSF) {
            telem_init(telem_cer, telem_pin |
                #if defined(MAIN_SIGNAL_PA2)
                    TELEMPIN_PA2_WAS_CRSF
                #elif defined(MAIN_SIGNAL_PB4)
                    TELEMPIN_PB6_WAS_CRSF
                #endif
                );
        }
        else if (cfg.input_mode == INPUTMODE_CRSF_SWCLK) {
            telem_init(telem_cer, telem_pin | TELEMPIN_PA14_WAS_CRSF);
        }
        else {
            if (telem_cer == NULL) {
                telem_cer = new Cereal_USART();
                telem_cer->init(CEREAL_ID_USART1, (cfg.telemetry_baud == 0) ? CRSF_BAUDRATE : cfg.telemetry_baud, false, false);
            }
            telem_init(telem_cer, telem_pin |
                #if defined(MAIN_SIGNAL_PA2)
                    TELEMPIN_PA2_WAS_PWM
                #elif defined(MAIN_SIGNAL_PB4)
                    TELEMPIN_PB4_WAS_PWM
                #endif
                | TELEMPIN_PB6_WAS_PWM);
        }
    }
    else // use SWCLK by default
    {
        if (cfg.input_mode == INPUTMODE_CRSF) {
            #if defined(MAIN_SIGNAL_PA2)
            telem_init(&main_cer, telem_pin |
                #if defined(MAIN_SIGNAL_PA2)
                    TELEMPIN_PA2_WAS_CRSF
                #elif defined(MAIN_SIGNAL_PB4)
                    TELEMPIN_PB6_WAS_CRSF
                #endif
                );
            #elif defined(MAIN_SIGNAL_PB4)
            telem_cer = new Cereal_USART();
            telem_cer->init(CEREAL_ID_USART_SWCLK, (cfg.telemetry_baud == 0) ? CRSF_BAUDRATE : cfg.telemetry_baud, false, false);
            telem_init(telem_cer,  telem_pin | TELEMPIN_PB6_WAS_CRSF);
            #endif
        }
        else if (cfg.input_mode == INPUTMODE_CRSF_SWCLK) {
            telem_init(&main_cer, telem_pin | TELEMPIN_PA14_WAS_CRSF);
        }
        else {
            // RC signal input is being used so SWCLK needs a full initialization
            main_cer.init(CEREAL_ID_USART_SWCLK, (cfg.telemetry_baud == 0) ? CRSF_BAUDRATE : cfg.telemetry_baud, false, false);
            telem_init(&main_cer, telem_pin | 
                #if defined(MAIN_SIGNAL_PA2)
                    TELEMPIN_PA2_WAS_PWM
                #elif defined(MAIN_SIGNAL_PB4)
                    TELEMPIN_PB4_WAS_PWM
                #endif
            );
        }
    }
}

// simply caches some items to speed up the polled task
static void telem_init(Cereal_USART* cer, uint8_t swap_mode)
{
    telem_cer = cer;
    telem_swapmode = swap_mode;
    telem_period = 1000 / cfg.telemetry_rate;
    telem_datamask = cfg.telemetry_port & 0xF0;
}

static void telem_task(void)
{
    if (telem_cer == NULL || telem_period <= 0) { // do nothing if telemetry is not configured
        return;
    }
    uint32_t now = millis();
    if ((now - telem_lastTime) < telem_period) { // send only when required by time interval
        return;
    }
    telem_lastTime = now;

    if ((telem_datamask & TELEMPORT_FLAG_DISABLE_CAPACITY) == 0)
    {
        if ((now - telem_mAH_time) >= 1000)
        {
            // process the current measurement accumulator every second
            telem_mAH_time = now;
            telem_mAs += sense_current_accum / sense_current_accum_cnt; // average mA use for one second
            telem_mAH_maxCnt = sense_current_accum_cnt > telem_mAH_maxCnt ? sense_current_accum_cnt : telem_mAH_maxCnt; // remember maximum number of samples per second
            sense_current_accum     = 0; // clear the individual sample accumulator
            sense_current_accum_cnt = 0; // clear the individual sample accumulator
        }
        uint32_t mAH = (telem_mAs + (sense_current_accum / telem_mAH_maxCnt)) / (60 * 60);
        telem_mAH_max = mAH > telem_mAH_max ? mAH : telem_mAH_max; // make sure number only goes up
    }

    uint8_t batt_telem_soc = 0;
    if ((telem_datamask & TELEMPORT_FLAG_DISABLE_SOC) == 0)
    {
        if (voltage_limit != 0) {
            batt_telem_soc = fi_map(sense_voltage, voltage_limit, batt_starting_voltage, 0, 100, true);
        }
        else {
            // voltage limit has not been calculated, so do it now
            battery_calc();
            if (batt_cell_cnt > 0) {
                uint32_t low_cell = (cfg.cell_max_volt * 3) / 4;
                batt_telem_soc = fi_map(sense_voltage, batt_cell_cnt * low_cell, batt_cell_cnt * cfg.cell_max_volt, 0, 100, true);
            }
        }
    }

    telem_pinPrep();

    static uint8_t telem_buffer[16];
    crsf_header_t* hdr = (crsf_header_t*)telem_buffer;
    hdr->sync = CRSF_SYNC_BYTE;
    hdr->type = CRSF_FRAMETYPE_BATTERY_SENSOR;
    uint32_t current = fi_div_rounded(sense_current, 100); // report in dV, so convert from mV
    uint32_t voltage = fi_div_rounded(sense_voltage, 100); // report in dA, so convert from mA
    if ((telem_datamask & TELEMPORT_FLAG_DISABLE_VOLTAGE) != 0) {
        voltage = 0;
    }
    if ((telem_datamask & TELEMPORT_FLAG_DISABLE_CURRENT) != 0) {
        current = 0;
    }
    TELEM_BIGENDIAN16(telem_buffer, 3, current);
    TELEM_BIGENDIAN16(telem_buffer, 5, voltage);
    TELEM_BIGENDIAN24(telem_buffer, 7, telem_mAH_max);
    telem_buffer[10] = batt_telem_soc;
    hdr->len = 12;
    telem_buffer[11] = crsf_crc8((const uint8_t *)telem_buffer, hdr->len - 1);
    telem_cer->writeb((uint8_t*)telem_buffer, hdr->len); // send the data into FIFO, which starts the actual transmission
    // wait until USART actually finishes
    while (telem_cer->is_tx_busy()) {
        sense_task();
    }
    // NOTE: this transmission is blocking
    // this should be fine as it is definitely shorter than a single CRSF incoming packet
    // so it will never occupy more than one useful loop time

    telem_pinRestore();

    // note: cereal_dmaRestart() will be called outside this function if needed
}

static void telem_pinPrep(void)
{
    if ((telem_swapmode & TELEMPIN_PA2_WAS_CRSF) != 0) {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_2;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_INPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_mode_QUICK(GPIOA, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_PINS_2);
        #endif
    }
    if ((telem_swapmode & TELEMPIN_PB6_WAS_CRSF) != 0 && (telem_swapmode & TELEMPIN_PA14_TO_TELEM) != 0) {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_6;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_INPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_mode_QUICK(GPIOB, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_PINS_6);
        #endif
    }
    if ((telem_swapmode & TELEMPIN_PB6_TO_TELEM) != 0) {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_0);
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_6;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE6, GPIO_MUX_0);
        gpio_mode_QUICK(GPIOB, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PINS_6);
        #endif
    }
    if ((telem_swapmode & TELEMPIN_PA14_TO_TELEM) != 0) {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_14, LL_GPIO_AF_1);
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_14;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE14, GPIO_MUX_1);
        gpio_mode_QUICK(GPIOA, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PINS_14);
        #endif
    }
    if ((telem_swapmode & TELEMPIN_PB6_TO_TELEM) != 0 || (telem_swapmode & TELEMPIN_PA14_TO_TELEM) != 0)
    {
        telem_cer->set_txrx_swap(false);
    }
}

static void telem_pinRestore(void)
{
    if ((telem_swapmode & TELEMPIN_PA2_WAS_CRSF) != 0) {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_2;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_mode_QUICK(GPIOA, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_PINS_2);
        #endif
    }
    if ((telem_swapmode & TELEMPIN_PA2_WAS_PWM) != 0) {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_0);
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_2;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE2, GPIO_MUX_0);
        gpio_mode_QUICK(GPIOA, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PINS_2);
        #endif
    }
    if ((telem_swapmode & TELEMPIN_PB6_WAS_CRSF) != 0) {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_6;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_mode_QUICK(GPIOB, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PINS_6);
        #endif
    }
    if ((telem_swapmode & TELEMPIN_PB6_WAS_CRSF) != 0) {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_6;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_mode_QUICK(GPIOB, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PINS_6);
        #endif
    }
    if ((telem_swapmode & TELEMPIN_PA14_WAS_PWM) != 0) {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_14;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_INPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_mode_QUICK(GPIOA, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PINS_14);
        #endif
    }
    if ((telem_swapmode & TELEMPIN_PB6_TO_TELEM) != 0 || (telem_swapmode & TELEMPIN_PA14_TO_TELEM) != 0)
    {
        telem_cer->set_txrx_swap(true);
    }
}

void telem_crsfTask(void)
{
    telem_callFromCrsf = true;
    telem_task();
}

void telem_mainTask(void)
{
    // if there is a need to swap, then only allow the task to execute from the CRSF reception
    if (telem_callFromCrsf || telem_swapmode == TELEMPIN_NO_NEED) {
        uint32_t now = millis();
        if (now < 3000) {
            return;
        }
        if ((now - telem_lastTime) < (telem_period * 5)) {
            return;
        }
    }
    telem_task();
}

#else

#endif
