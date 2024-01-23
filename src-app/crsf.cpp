#include "crsf.h"
#include "userconfig.h"

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

static Cereal* cereal;
static uint16_t crsf_channels[CRSF_CHAN_CNT] = {0};
static volatile bool     new_flag       = false;
static volatile uint32_t last_good_time = 0;
static volatile uint8_t  good_pulse_cnt = 0;
static volatile uint8_t  bad_pulse_cnt  = 0;

CrsfChannel::CrsfChannel(void)
{
}

void CrsfChannel::init(Cereal* cer, uint8_t idx)
{
    cereal = cer;
    _idx = idx;
}

void CrsfChannel::task(void)
{
    if (cereal->get_idle_flag(true) || (millis() - last_good_time) >= 100) // UART is idle or a long time has passed
    {
        // check packet header for match
        if (cereal->peek() == CRSF_SYNC_BYTE
            && cereal->peekAt(2) == CRSF_FRAMETYPE_RC_CHANNELS_PACKED
            && cereal->peekAt(1) >= (cereal->available() + 2)
        ) {
            uint8_t* buff = cereal->get_buffer();
            crsf_header_t* hdr = (crsf_header_t*)buff;
            uint8_t crc = crsf_crc8(&(hdr->type), hdr->len - 1);
            if (crc == buff[hdr->len + 1]) // CRC matches
            {
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

                good_pulse_cnt++;
                bad_pulse_cnt = 0;
                last_good_time = millis();
                new_flag = true;
                has_new = true;
                cereal->consume(hdr->len + 2); // pop the buffer
                if (arm_pulses_required > 0)
                {
                    if (read() == 0) {
                        arming_cnt++;
                        if (arming_cnt >= arm_pulses_required) {
                            armed = true;
                        }
                    }
                    else {
                        arming_cnt = 0;
                    }
                }
                else {
                    armed = true;
                }
            }
            else
            {
                if (bad_pulse_cnt < 3) {
                    bad_pulse_cnt++;
                }
                else {
                    good_pulse_cnt = 0;
                }
                arming_cnt = 0;
            }

            cereal->reset_buffer();
        }
        else {
            cereal->reset_buffer();
        }
    }
    if ((millis() - last_good_time) >= 200)
    {
        arming_cnt = 0;
        while (true)
        {
            int16_t d = cereal->peek();
            if (d == CRSF_SYNC_BYTE || d < 0) {
                break;
            }
            else {
                cereal->read();
            }
        }
    }

    if (disarm_timeout > 0)
    {
        if ((millis() - last_good_time) >= disarm_timeout)
        {
            armed = false;
            arming_cnt = 0;
        }
    }
}

int16_t CrsfChannel::read(void)
{
    if (_idx <= 0) {
        return 0;
    }

    uint32_t x = crsf_channels[_idx - 1];
    if (x == CRSF_CHANNEL_VALUE_MID) {
        return 0;
    }
    x = fi_map(x, CRSF_CHANNEL_VALUE_1000, CRSF_CHANNEL_VALUE_2000, cfg.rc_mid - cfg.rc_range, cfg.rc_mid + cfg.rc_range, false);
    return rc_pulse_map(x);
}

bool CrsfChannel::is_alive(void)
{
    if (_idx <= 0) {
        return false;
    }

    if ((millis() - last_good_time) < RC_INPUT_TIMEOUT)
    {
        if (good_pulse_cnt >= 3) {
            return true;
        }
    }
    new_flag = false;
    has_new = false;
    return false;
}

bool CrsfChannel::has_new(bool clr)
{
    if (_idx <= 0) {
        return false;
    }

    bool x = new_flag || has_new;
    if (clr) {
        new_flag = false;
        has_new = false;
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

uint16_t crsf_readChan(uint8_t i)
{
    return crsf_channels[i - 1];
}
