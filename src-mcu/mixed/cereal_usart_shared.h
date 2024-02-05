static fifo_t fifo_rx_1;
static fifo_t fifo_rx_2;
static fifo_t fifo_tx_1;
static fifo_t fifo_tx_2;
static volatile uint32_t last_rx_time_1 = 0;
static volatile uint32_t last_rx_time_2 = 0;
static volatile bool had_first_byte_1 = false;
static volatile bool had_first_byte_2 = false;

#if defined(DEBUG_PRINT)
static Cereal_USART* dbg_cer_tgt = NULL; // this is the port being used by all dbg_print() statements
#endif

static volatile bool is_idle_1 = false;
static volatile bool is_idle_2 = false;
static uint8_t use_dma_on = 0;
#define CEREAL_DMA_SIZE 64                // doesn't need to be long, we only process one packet at the head and then toss the rest
static uint8_t dma_buff[CEREAL_DMA_SIZE] __attribute__((aligned(4)));

extern uint8_t crsf_inputGuess;

void cereal_dmaRestart(void);

Cereal_USART::Cereal_USART(void)
{

}

void Cereal_USART::sw_init(uint8_t id)
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
        #elif defined(MAIN_SIGNAL_PA6)
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
        #if defined(STM32F051DISCO)
        //_usart = USART1;
        //_u = CEREAL_ID_USART1;
        _usart = USART2;
        _u = CEREAL_ID_USART2;
        #elif defined(STM32G071NUCLEO)
        _usart = USART2;
        _u = CEREAL_ID_USART2;
        #endif
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
        fifo_init(&fifo_rx_2, cer_buff_3, CEREAL_BUFFER_SIZE);
        fifo_rx = &fifo_rx_2;
        fifo_init(&fifo_tx_2, cer_buff_4, CEREAL_BUFFER_SIZE);
        fifo_tx = &fifo_tx_2;
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

void Cereal_USART::write(uint8_t x)
{
    fifo_push(fifo_tx, x);
    if (LL_USART_IsActiveFlag_TXE(_usart)) {
        uint8_t y = fifo_pop(fifo_tx);
        LL_USART_TransmitData8(_usart, y);
    }
    if (x == '\n') {
        flush();
    }
    echo_time = millis();
}

void cereal_dmaRestart(void)
{
    // reset the DMA buffer completely or else CRSF parsing will fail
    LL_DMA_DisableChannel(CEREAL_DMAx, CEREAL_DMA_CHAN);
    LL_DMA_SetDataLength(CEREAL_DMAx, CEREAL_DMA_CHAN, 0);
    dma_buff[0] = 0; // invalidate the packet
    dma_buff[1] = 0; // invalidate the packet
    dma_buff[2] = 0; // invalidate the packet
    dma_buff[3] = 0; // invalidate the packet
    LL_DMA_SetDataLength(CEREAL_DMAx, CEREAL_DMA_CHAN, CEREAL_DMA_SIZE);
    if (use_dma_on == CEREAL_ID_USART1) {
        LL_USART_EnableDMAReq_RX(USART1);
    }
    else if (use_dma_on == CEREAL_ID_USART2) {
        LL_USART_EnableDMAReq_RX(USART2);
    }
    LL_DMA_EnableChannel(CEREAL_DMAx, CEREAL_DMA_CHAN);
}

void USARTx_IRQHandler(uint8_t _u, USART_TypeDef* usart, fifo_t* fifo_rx, fifo_t* fifo_tx, volatile bool* is_idle, volatile uint32_t* timestamp, volatile bool* had_1st)
{
    if (LL_USART_IsActiveFlag_RXNE(usart))
    {
        dbg_evntcnt_add(DBGEVNTID_USART_RX);
        volatile uint8_t x = LL_USART_ReceiveData8(usart);
        if ((x < 0x80 && x != 0x00) || (*had_1st) != false) { // this prevents the first junk character from entering the FIFO
            fifo_push(fifo_rx, x);
            *had_1st = true;
        }
        *timestamp = millis();
    }
    if (LL_USART_IsActiveFlag_TC(usart))
    {
        dbg_evntcnt_add(DBGEVNTID_USART_TX);
        LL_USART_ClearFlag_TC(usart);
        if (fifo_available(fifo_tx))
        {
            uint8_t x = fifo_pop(fifo_tx);
            LL_USART_TransmitData8(usart, x);
        }
        #if defined(STMICRO)
        else
        {
            LL_USART_EnableDirectionRx(usart);
        }
        #endif
    }
    if (LL_USART_IsActiveFlag_IDLE(usart))
    {
        dbg_evntcnt_add(DBGEVNTID_USART_IDLE);
        LL_USART_ClearFlag_IDLE(usart);
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
    #ifdef MCU_F051
    NVIC_ClearPendingIRQ(USART1_IRQn);
    #endif
}

void USART2_IRQHandler(void)
{
    USARTx_IRQHandler(CEREAL_ID_USART2, USART2, &fifo_rx_2, &fifo_tx_2, (volatile bool*)&is_idle_2, (volatile uint32_t*)&last_rx_time_2, (volatile bool*)&had_first_byte_2);
    #ifdef MCU_F051
    NVIC_ClearPendingIRQ(USART2_IRQn);
    #endif
}

#ifdef __cplusplus
}
#endif

#if defined(DEBUG_PRINT)

void debug_writechar_cpp(char x)
{
    if (dbg_cer_tgt != NULL) {
        dbg_cer_tgt->write(x);
    }
}

int debug_printf_cpp(const char *format, va_list arg)
{
    if (dbg_cer_tgt != NULL) {
        return dbg_cer_tgt->vprintf(format, arg);
    }
    return 0;
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

int debug_printf(const char *format, va_list arg)
{
    return debug_printf_cpp(format, arg);
}

#ifdef __cplusplus
}
#endif
#endif
