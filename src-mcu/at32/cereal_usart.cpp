#include "cereal.h"

#define CEREAL_DMAx        DMA1
#define CEREAL_DMA_CHAN    DMA1_CHANNEL5

static fifo_t fifo_rx_1;
static fifo_t fifo_rx_2;
static fifo_t fifo_tx_1;
static fifo_t fifo_tx_2;
static volatile uint32_t last_rx_time_1 = 0;
static volatile uint32_t last_rx_time_2 = 0;
static volatile bool had_first_byte_1 = false;
static volatile bool had_first_byte_2 = false;

#if defined(DEBUG_PRINT)
static Cereal_USART* dbg_cer_tgt = NULL;
#endif

static volatile bool is_idle_1 = false;
static volatile bool is_idle_2 = false;
static uint8_t use_dma_on = 0;
#define CEREAL_DMA_SIZE 64                // doesn't need to be long, we only process one packet at the head and then toss the rest
static uint8_t dma_buff[CEREAL_DMA_SIZE];

void cereal_dmaRestart(void)
{
    // reset the DMA buffer completely or else CRSF parsing will fail
    dma_channel_enable (CEREAL_DMA_CHAN, FALSE);
    dma_data_number_set(CEREAL_DMA_CHAN, 0);
    dma_buff[0] = 0; // invalidate the packet
    dma_buff[1] = 0; // invalidate the packet
    dma_buff[2] = 0; // invalidate the packet
    dma_buff[3] = 0; // invalidate the packet
    dma_data_number_set(CEREAL_DMA_CHAN, CEREAL_DMA_SIZE);
    if (use_dma_on == CEREAL_ID_USART1) {
        usart_dma_receiver_enable(USART1, TRUE);
    }
    else if (use_dma_on == CEREAL_ID_USART2) {
        usart_dma_receiver_enable(USART2, TRUE);
    }
    dma_channel_enable(CEREAL_DMA_CHAN, TRUE);
}

void USARTx_IRQHandler(uint8_t _u, usart_type* usart, fifo_t* fifo_rx, fifo_t* fifo_tx, volatile bool* is_idle, volatile uint32_t* timestamp, volatile bool* had_1st)
{
    if (usart_flag_get(usart, USART_RDBF_FLAG))
    {
        dbg_evntcnt_add(DBGEVNTID_USART_RX);
        usart_flag_clear(usart, USART_RDBF_FLAG);
        volatile uint8_t x = usart_data_receive(usart);
        if ((x < 0x80 && x != 0x00) || (*had_1st) != false) { // this prevents the first junk character from entering the FIFO
            fifo_push(fifo_rx, x);
            *had_1st = true;
        }
        *timestamp = millis();
    }
    if (usart_flag_get(usart, USART_TDC_FLAG))
    {
        dbg_evntcnt_add(DBGEVNTID_USART_TX);
        usart_flag_clear(usart, USART_TDC_FLAG);
        if (fifo_available(fifo_tx))
        {
            uint8_t x = fifo_pop(fifo_tx);
            usart_data_transmit(usart, x);
        }
    }
    if (usart_flag_get(usart, USART_IDLEF_FLAG))
    {
        dbg_evntcnt_add(DBGEVNTID_USART_IDLE);
        usart_flag_clear(usart, USART_IDLEF_FLAG);
        *is_idle = true;
        *timestamp = millis();
    }
}

#ifdef __cplusplus
extern "C" {
#endif

void USART1_IRQHandler(void)
{
    USARTx_IRQHandler(CEREAL_ID_USART1, USART1, &fifo_rx_1, &fifo_tx_1, (volatile bool*)&is_idle_1, (volatile uint32_t*)&last_rx_time_1, (volatile bool*)&had_first_byte_1);
    // NVIC_ClearPendingIRQ(USART1_IRQn);
}

void USART2_IRQHandler(void)
{
    USARTx_IRQHandler(CEREAL_ID_USART2, USART2, &fifo_rx_2, &fifo_tx_2, (volatile bool*)&is_idle_2, (volatile uint32_t*)&last_rx_time_2, (volatile bool*)&had_first_byte_2);
    // NVIC_ClearPendingIRQ(USART2_IRQn);
}

#ifdef __cplusplus
}
#endif

Cereal_USART::Cereal_USART(void)
{

}

extern uint8_t crsf_inputGuess;

void Cereal_USART::init(uint8_t id, uint32_t baud, bool invert, bool halfdup, bool swap, bool dma)
{
    _id = id;
    if (id == CEREAL_ID_USART_CRSF)
    {
        #if defined(MAIN_SIGNAL_PA2)
        if (crsf_inputGuess == 1)
        {
            _usart = USART1;
            _u = CEREAL_ID_USART1;
        }
        else if (crsf_inputGuess == 2)
        {
            _usart = USART2;
            _u = CEREAL_ID_USART2;
        }
        // the main loop will never let crsf_inputGuess be zero
        #elif defined(MAIN_SIGNAL_PB4)
        _usart = USART1;
        _u = CEREAL_ID_USART1;
        #else
        _usart = USART2;
        _u = CEREAL_ID_USART2;
        #endif
        _id = _u;
    }

    if (id == CEREAL_ID_USART1) {
        _usart = USART1;
        _u = CEREAL_ID_USART1;
    }
    else if (id == CEREAL_ID_USART2) {
        _usart = USART2;
        _u = CEREAL_ID_USART2;
    }
    else if (_id == CEREAL_ID_USART_DEBUG) {
        // TODO
    }
    else if (_id == CEREAL_ID_USART_SWCLK) {
        _usart = USART2;
        _u = CEREAL_ID_USART2;
    }

    if (_u == CEREAL_ID_USART1) {
        fifo_init(&fifo_rx_1, cer_buff_1, CEREAL_BUFFER_SIZE);
        fifo_rx = &fifo_rx_1;
        fifo_init(&fifo_tx_1, cer_buff_2, CEREAL_BUFFER_SIZE);
        fifo_tx = &fifo_tx_1;
    }
    else if (_u == CEREAL_ID_USART2) {
        fifo_init(&fifo_rx_2, cer_buff_1, CEREAL_BUFFER_SIZE);
        fifo_rx = &fifo_rx_2;
        fifo_init(&fifo_tx_2, cer_buff_2, CEREAL_BUFFER_SIZE);
        fifo_tx = &fifo_tx_2;
    }

    _usart->baudr = CLK_CNT(baud);
    uint32_t tmpreg;

    tmpreg = (1 << 3);
    if (halfdup) {
        _usart->ctrl3 |= tmpreg;
    }
    else {
        _usart->ctrl3 &= ~tmpreg;
    }

    uint32_t cr2 = _usart->ctrl2;
    tmpreg = (1 << 15);
    if (swap) {
        cr2 |= tmpreg;
    }
    else {
        cr2 &= ~tmpreg;
    }
    _usart->ctrl2 = cr2;

    // NOTE: AT32 does not have the invert function

    uint32_t cr1 = _usart->ctrl1;

    if (dma) {
        cr1 |= (1 << 2) | (1 << 4); // receiver enable, idle interrupt enable
    }
    else
    {
        cr1 |= (1 << 3) | (1 << 6) | (1 << 2) | (1 << 5); // tx enable, tx-complete interrupt enable, rx enable, rx buffer-not-empty interrupt enable
    }
    _usart->ctrl1 = cr1;

    if (_id == CEREAL_ID_USART1)
    {
        gpio_mode_QUICK    (GPIOB, GPIO_MODE_MUX, GPIO_PULL_UP, GPIO_PINS_6);
        gpio_pin_mux_config(GPIOB, GPIO_PINS_SOURCE6, GPIO_MUX_0);
    }
    else if (_id == CEREAL_ID_USART2)
    {
        gpio_mode_QUICK    (GPIOA, GPIO_MODE_MUX, GPIO_PULL_UP, GPIO_PINS_2);
        gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE2, GPIO_MUX_1);
    }
    else if (_id == CEREAL_ID_USART_SWCLK)
    {
        gpio_mode_QUICK    (GPIOA, GPIO_MODE_MUX, GPIO_PULL_UP, GPIO_PIN_SWCLK);
        gpio_pin_mux_config(GPIOA, GPIO_PINS_SOURCE14, GPIO_MUX_1);
        gpio_mode_QUICK    (GPIOA, GPIO_MODE_INPUT, GPIO_PULL_NONE, GPIO_PIN_SWDIO); // initialize the SWDIO pin as a floating input, which means the user can solder to both pads
    }
    #if defined(DEVELOPMENT_BOARD)
    else if (_id == CEREAL_ID_USART_DEBUG)
    {
        // TODO
        dbg_cer_tgt = this;
    }
    #endif

    if (dma) {
        use_dma_on = _u;
        if (_u == CEREAL_ID_USART1) {
            scfg_usart1_rx_dma_channel_remap(SCFG_USART1_RX_TO_DMA_CHANNEL_5);
        }
        dma_init_type dmacfg;
        dma_default_para_init(&dmacfg);
        dmacfg.peripheral_base_addr   = (uint32_t)&(_usart->dt);
        dmacfg.memory_base_addr       = (uint32_t)dma_buff;
        dmacfg.direction              = DMA_DIR_PERIPHERAL_TO_MEMORY;
        dmacfg.buffer_size            = CEREAL_DMA_SIZE;
        dmacfg.peripheral_inc_enable  = FALSE;
        dmacfg.memory_inc_enable      = TRUE;
        dmacfg.peripheral_data_width  = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
        dmacfg.memory_data_width      = DMA_MEMORY_DATA_WIDTH_BYTE;
        dmacfg.loop_mode_enable       = FALSE;
        dmacfg.priority               = DMA_PRIORITY_HIGH;
        dma_init(CEREAL_DMA_CHAN, &dmacfg);

        cereal_dmaRestart();
    }

    usart_enable(_usart, TRUE);

    IRQn_Type irqn = _u == CEREAL_ID_USART2 ? USART2_IRQn : USART1_IRQn;

    NVIC_SetPriority(irqn, 1);
    NVIC_EnableIRQ(irqn);
}

void Cereal_USART::write(uint8_t x)
{
    fifo_push(fifo_tx, x);
    if (usart_flag_get(_usart, USART_TDBE_FLAG)) {
        usart_flag_clear(_usart, USART_TDBE_FLAG);
        uint8_t y = fifo_pop(fifo_tx);
        usart_data_transmit(_usart, y);
    }
    if (x == '\n') {
        flush();
    }
}

void Cereal_USART::flush(void)
{
    while (fifo_available(fifo_tx)) {
        // do nothing but wait
    }
}

uint32_t Cereal_USART::get_last_time(void)
{
    if (_u == CEREAL_ID_USART1) {
        return last_rx_time_1;
    }
    else if (_u == CEREAL_ID_USART2) {
        return last_rx_time_2;
    }
    return 0;
}

uint8_t* Cereal_USART::get_buffer(void)
{
    if (use_dma_on != _u) {
        return cer_buff_1;
    }
    else {
        return dma_buff;
    }
}

bool Cereal_USART::get_idle_flag(bool clr)
{
    bool x = false;
    __disable_irq();
    if (_u == CEREAL_ID_USART1) {
        x = is_idle_1;
        if (clr) {
            is_idle_1 = false;
        }
    }
    else if (_u == CEREAL_ID_USART2) {
        x = is_idle_2;
        if (clr) {
            is_idle_2 = false;
        }
    }
    __enable_irq();
    return x;
}

#if defined(DEBUG_PRINT)

void debug_writechar_cpp(char x)
{
    if (dbg_cer_tgt != NULL) {
        dbg_cer_tgt->write(x);
    }
}

#ifdef __cplusplus
extern "C" {
#endif

void debug_writechar(char x) {
    debug_writechar_cpp(x);
}

int debug_writebuff(uint8_t* buf, int len) {
    for (int i = 0; i < len; i++) {
        debug_writechar_cpp(buf[i]);
    }
    return len;
}

#ifdef __cplusplus
}
#endif

#endif
