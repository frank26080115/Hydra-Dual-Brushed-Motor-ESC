#include "crsf.h"
#include "userconfig.h"

//#define DEBUG_CRSF
//#define DEBUG_CRSF_RATE

// reference https://github.com/ExpressLRS/ExpressLRS/blob/69381f22b87bdc1056bcb8b2f4ecd08fd214d356/src/lib/CrsfProtocol/crsf_protocol.h
#define CRSF_CHAN_CNT 16
#define CRSF_SYNC_BYTE 0xC8
#define CRSF_FRAMETYPE_RC_CHANNELS_PACKED 0x16

typedef struct
{
    uint8_t sync;
    uint8_t len;
    uint8_t type;
}
__attribute__((packed))
crsf_header_t;

uint8_t crsf_crc8(const uint8_t *ptr, uint8_t len);

uint8_t crsf_inputGuess = 0; // 0 means unknown, 1 means PB6 USART1, 2 means PA2 USART2

static Cereal_USART* cereal;
static uint16_t crsf_channels[CRSF_CHAN_CNT] __attribute__((aligned(4))) = {0};
static bool     new_flag       = false;
static uint32_t last_good_time = 0;
static uint32_t last_any_time  = 0;
static uint8_t  good_pulse_cnt = 0;
static uint8_t  bad_pulse_cnt  = 0;

#ifdef DEBUG_CRSF_RATE
static uint32_t data_rate_cnt = 0;
static uint32_t data_rate_tmr = 0;
static uint32_t calls_cnt = 0;
static uint32_t badcrc_cnt = 0;
static uint32_t badhdr_cnt = 0;
static uint32_t idle_cnt_prev = 0;
#endif

extern void cereal_dmaRestart(void);

CrsfChannel::CrsfChannel(void)
{
}

void CrsfChannel::init(Cereal_USART* cer, uint8_t idx)
{
    cereal = cer;
    _idx = idx;
}

void CrsfChannel::task(void)
{
    #ifdef DEBUG_CRSF
    static bool to_debug = false;
    static uint32_t last_debug_time = 0;
    if ((millis() - last_debug_time) >= 200 && to_debug == false) {
        last_debug_time = millis();
        to_debug = true;
    }
    #endif

    #ifdef DEBUG_CRSF_RATE
    calls_cnt++;
    #endif

    uint32_t now = millis();

    bool           is_idle = cereal->get_idle_flag(true); // the DMA does not actually know the size of a packet, each packet is variable in length, so we simply wait until the USART detects the bus is idle
    uint8_t*       buff    = cereal->get_buffer();
    crsf_header_t* hdr     = (crsf_header_t*)buff; // makes the data easy to parse

    if (is_idle) {
        last_any_time = now;
    }

    if (is_idle && hdr->sync == CRSF_SYNC_BYTE && hdr->type == CRSF_FRAMETYPE_RC_CHANNELS_PACKED)
    {
        uint8_t crc = crsf_crc8(&(hdr->type), hdr->len - 1); // check if CRC matches, this would fail if the length byte is invalid
        if (crc == buff[hdr->len + 1]) // CRC matches
        {
            #ifdef DEBUG_CRSF
            if (to_debug) {
                dbg_printf("[%u] CRSF good (last %u)\r\n", now, last_good_time);
                to_debug = false;
            }
            #endif

            // decompress, 11 bits per channel
            if (hdr->len >= 8) {
                crsf_channels[0]  = ((buff[3]       | buff[4]  << 8) & 0x07FF);
                crsf_channels[1]  = ((buff[4]  >> 3 | buff[5]  << 5) & 0x07FF);
                crsf_channels[2]  = ((buff[5]  >> 6 | buff[6]  << 2 | buff[7] << 10) & 0x07FF);
                crsf_channels[3]  = ((buff[7]  >> 1 | buff[8]  << 7) & 0x07FF);
            }
            if (hdr->len >= 13) {
                crsf_channels[4]  = ((buff[8]  >> 4 | buff[9]  << 4) & 0x07FF);
                crsf_channels[5]  = ((buff[9]  >> 7 | buff[10] << 1 | buff[11] << 9) & 0x07FF);
                crsf_channels[6]  = ((buff[11] >> 2 | buff[12] << 6) & 0x07FF);
                crsf_channels[7]  = ((buff[12] >> 5 | buff[13] << 3) & 0x07FF);
            }
            if (hdr->len >= 19) {
                crsf_channels[8]  = ((buff[14]      | buff[15] << 8) & 0x07FF);
                crsf_channels[9]  = ((buff[15] >> 3 | buff[16] << 5) & 0x07FF);
                crsf_channels[10] = ((buff[16] >> 6 | buff[17] << 2 | buff[18] << 10) & 0x07FF);
                crsf_channels[11] = ((buff[18] >> 1 | buff[19] << 7) & 0x07FF);
            }
            if (hdr->len >= 24) {
                crsf_channels[12] = ((buff[19] >> 4 | buff[20] << 4) & 0x07FF);
                crsf_channels[13] = ((buff[20] >> 7 | buff[21] << 1 | buff[22] << 9) & 0x07FF);
                crsf_channels[14] = ((buff[22] >> 2 | buff[23] << 6) & 0x07FF);
                crsf_channels[15] = ((buff[23] >> 5 | buff[24] << 3) & 0x07FF);
            }

            rc_register_good_pulse(
                0
                , 0, 0
                , &last_good_time
                , &good_pulse_cnt, &bad_pulse_cnt, NULL
                , (bool*)&new_flag, NULL
            );

            #ifdef DEBUG_CRSF_RATE
            data_rate_cnt++;
            if ((now - data_rate_tmr) >= 1000) {
                data_rate_tmr = now;
                uint32_t idle_cnt_now = dbg_evntcnt_get(DBGEVNTID_USART_IDLE);
                dbg_printf("CRSF %u / %u / %u / %u / %u\r\n", data_rate_cnt, badcrc_cnt, badhdr_cnt, idle_cnt_now - idle_cnt_prev, calls_cnt);
                data_rate_cnt = 0;
                calls_cnt = 0;
                badcrc_cnt = 0;
                badhdr_cnt = 0;
                idle_cnt_prev = idle_cnt_now;
            }
            #endif

        }
        else // bad CRC
        {
            #ifdef DEBUG_CRSF
            if (to_debug) {
                dbg_printf("CRSF bad CRC 0x%02X != 0x%02X  --  ", crc, buff[hdr->len + 1]);
                for (int iii = 0; iii <= hdr->len + 1; iii++) {
                    if (iii < 3 || iii > hdr->len - 3) {
                        dbg_printf("%02X ", buff[iii]);
                    }
                }
                dbg_printf("\r\n");
                to_debug = false;
            }
            #endif
            #ifdef DEBUG_CRSF_RATE
            badcrc_cnt++;
            #endif

            rc_register_bad_pulse(&good_pulse_cnt, &bad_pulse_cnt, NULL);
        }
    }
    else if (is_idle && hdr->sync == CRSF_SYNC_BYTE && hdr->type != CRSF_FRAMETYPE_RC_CHANNELS_PACKED) // some other packet type is received, throw it out
    {
        #ifdef DEBUG_CRSF
        if (to_debug) {
            dbg_printf("CRSF unwanted packet 0x%02X 0x%02X 0x%02X\r\n",
                    hdr->sync, hdr->len, hdr->type
                );
            to_debug = false;
        }
        #endif
    }
    else if (is_idle) // dunno what happened here but throw out the packet
    {
        #ifdef DEBUG_CRSF_RATE
        badhdr_cnt++;
        #endif
    }

    if (is_idle) {
        cereal_dmaRestart(); // restart the DMA and get reading to receive another packet
    }
    else {
        // if a long time has passed without a packet, restart UART and DMA
        if ((now - last_any_time) >= 30) {
            last_any_time = now;
            cereal->restart();
            cereal_dmaRestart();
        }
    }

    _has_new |= new_flag;
    now = millis();

    if (arm_pulses_required > 0) // user has configured a requirement
    {
        if ((now - arming_tick) >= 20) // time to check
        {
            arming_tick = now; // stage next time to check

            if ((now - last_good_time) <= 100 && read() == 0) { // signal is still valid and reading 0
                arming_cnt++;
                if (arming_cnt >= arm_pulses_required) { // met requirements
                    #ifdef DEBUG_CRSF
                    if (armed == false) {
                        dbg_printf("CRSF armed!\r\n");
                    }
                    #endif
                    armed = true;
                }
            }
            else {
                #ifdef DEBUG_CRSF
                if (read() == 0) {
                    dbg_printf("CRSF temporary lost arming count (%u - %u)\r\n", now, last_good_time);
                }
                #endif
                arming_cnt = 0;
            }
        }
    }
    else { // user specified no arming is required
        // arm if any signal is available
        armed |= ((millis() - last_good_time) < RC_INPUT_TIMEOUT && good_pulse_cnt >= 3);
    }

    uint32_t disarm_timeout_used = disarm_timeout > 0 ? disarm_timeout : 3000; // if user specified, then use value, otherwise, disarm after 3 seconds
    if ((now - last_good_time) >= disarm_timeout_used) // passed the timeout
    {
        #ifdef DEBUG_CRSF
        dbg_printf("CRSF disarmed due to timeout (%u - %u > %u)\r\n", now, last_good_time, disarm_timeout);
        #endif
        armed = false;
        arming_cnt = 0;
    }
}

int16_t CrsfChannel::read(void)
{
    if (_idx <= 0) {
        return 0; // unconfigured
    }

    uint32_t x = crsf_channels[_idx - 1];
    if (x == CRSF_CHANNEL_VALUE_MID) {
        return 0;
    }
    x = fi_map(x, CRSF_CHANNEL_VALUE_1000, CRSF_CHANNEL_VALUE_2000, cfg.rc_mid - cfg.rc_range, cfg.rc_mid + cfg.rc_range, false);
    // the range is mapped to be similar to a RC pulse
    // this way rc_pulse_map can handle the centering and deadzone
    return rc_pulse_map(x);
}

int16_t CrsfChannel::readRaw(void)
{
    if (_idx <= 0) {
        return 0; // unconfigured
    }

    return crsf_channels[_idx - 1];
}

bool CrsfChannel::is_alive(void)
{
    if (_idx <= 0) {
        return false; // unconfigured
    }

    if ((millis() - last_good_time) < RC_INPUT_TIMEOUT)
    {
        if (good_pulse_cnt >= 3) {
            return true;
        }
    }
    new_flag = false;
    _has_new = false;
    return false;
}

bool CrsfChannel::has_new(bool clr)
{
    if (_idx <= 0) {
        return false; // unconfigured
    }

    bool x = new_flag || _has_new;
    if (clr) {
        new_flag = false;
        _has_new = false;
    }
    return x;
}

bool CrsfChannel::is_armed(void)
{
    return armed;
}

void CrsfChannel::disarm(void)
{
    armed = false;
    arming_cnt = 0;
}

#ifdef __cplusplus
extern "C" {
#endif

uint16_t crsf_readChan(uint8_t i)
{
    if (i <= 0) {
        return 0; // unconfigured
    }
    return crsf_channels[i - 1];
}

#ifdef __cplusplus
}
#endif

#ifdef RC_LOG_JITTER
uint32_t CrsfChannel::readJitter(void) { return 0; } // reads 0 for CRSF, any noise can be measured from the transmitter
#endif
