#include "main.h"
#include "telemetry.h"
#include "crsf.h"
#include "sense.h"
#include "userconfig.h"
#include "stm32_at32_compat.h"

#ifdef ENABLE_TELEMETRY

extern "C"
{
    extern uint32_t sense_current;
    extern uint32_t sense_voltage;
    extern uint8_t  batt_cell_cnt;
    extern uint32_t batt_starting_voltage;
    extern uint32_t sense_current_accum;
    extern uint32_t sense_current_accum_cnt;
}

static Cereal_USART* telem_cer = NULL;
static uint8_t telem_swapmode;
static uint32_t telem_period;
static uint32_t telem_lastTime = 0;
static uint32_t telem_mAs = 0;
static uint32_t telem_mAH_max = 0;
static uint32_t telem_mAH_time = 0;
static uint32_t telem_mAH_maxCnt = 0;

extern uint8_t crsf_crc8(const uint8_t *ptr, uint8_t len);

#define TELEM_BIGENDIAN16(_buf, _idx, _val)    do {(_buf)[_idx] = (_val) >> 8;  (_buf)[(_idx) + 1] = (_val) & 0xFF;} while (0)
#define TELEM_BIGENDIAN24(_buf, _idx, _val)    do {(_buf)[_idx] = (_val) >> 16; (_buf)[(_idx) + 1] = ((_val) >> 8) & 0xFF; (_buf)[(_idx) + 2] = (_val) & 0xFF;} while (0)

void telem_init(Cereal_USART* cer, uint8_t swap_mode)
{
    if (cfg.telemetry_rate <= 0) {
        return;
    }
    telem_cer = cer;
    telem_swapmode = swap_mode;
    telem_period = 1000 / cfg.telemetry_rate;
}

void telem_task(void)
{
    if (telem_cer == NULL || telem_period <= 0) {
        return;
    }
    uint32_t now = millis();
    if ((now - telem_lastTime) < telem_period)
    {
        return;
    }
    telem_lastTime = now;

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

    uint8_t batt_telem_soc = 0;
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

    // swap pin function
    if (telem_swapmode == TELEMSWAP_PA2_PA14)
    {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_2;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_INPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_14, LL_GPIO_AF_1);
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_14;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_mode_QUICK(GPIOA, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_PINS_2);
        gpio_mode_QUICK(GPIOA, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PINS_14);
        gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE14, GPIO_MUX_1);
        #endif
    }
    // disable swap on USART and enable transmissions
    #if defined(STMICRO)
    USART2->CR2 &= ~USART_CR2_SWAP;
    USART2->CR1 |= USART_CR1_TE | USART_CR1_TCIE;
    #elif defined(ARTERY)
    USART2->ctrl2 &= ~(1<<15);
    USART2->ctrl1 |= (1 << 3) | (1 << 6);
    #endif

    static uint8_t telem_buffer[16];
    crsf_header_t* hdr = (crsf_header_t*)telem_buffer;
    hdr->sync = CRSF_SYNC_BYTE;
    hdr->type = CRSF_FRAMETYPE_BATTERY_SENSOR;
    uint32_t current = fi_div_rounded(sense_current, 100); // report in dV, so convert from mV
    uint32_t voltage = fi_div_rounded(sense_voltage, 100); // report in dA, so convert from mA
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

    // swap the pin functions back
    if (telem_swapmode == TELEMSWAP_PA2_PA14)
    {
        #if defined(STMICRO)
        LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_2;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        GPIO_InitStruct.Pin   = LL_GPIO_PIN_14;
        GPIO_InitStruct.Mode  = LL_GPIO_MODE_INPUT;
        GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #elif defined(ARTERY)
        gpio_mode_QUICK(GPIOA, GPIO_MODE_INPUT, GPIO_PULL_UP, GPIO_PINS_14);
        gpio_mode_QUICK(GPIOA, GPIO_MODE_MUX, GPIO_PULL_NONE, GPIO_PINS_2);
        #endif
    }
    // restore UART
    #if defined(STMICRO)
    USART2->CR2 |= USART_CR2_SWAP;
    #elif defined(ARTERY)
    USART2->ctrl2 |= (1<<15);
    #endif

    // note: cereal_dmaRestart() will be called outside this function if needed
}

void telem_crsfTask(void)
{
    telem_task();
}

void telem_mainTask(void)
{
    // if there is a need to swap, then only allow the task to execute from the CRSF reception
    if (telem_swapmode != TELEMSWAP_NO_NEED) {
        return;
    }
    telem_task();
}

#else

#endif
