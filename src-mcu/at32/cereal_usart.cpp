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

Cereal_USART::Cereal_USART(void)
{

}

extern uint8_t crsf_inputGuess;

void Cereal_USART::init(uint8_t id, uint32_t baud, bool invert, bool halfdup, bool swap, bool dma)
{
    _id = id;
    if (id == CEREAL_ID_USART_CRSF)
    {
        #if INPUT_PIN == GPIO_PINS_2
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
        #elif INPUT_PIN == GPIO_PINS_4
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

    _usart->BRR = CLK_CNT(baud);

    if (halfdup) {
        _usart->CR3 |= USART_CR3_HDSEL;
    }
    else {
        _usart->CR3 &= ~USART_CR3_HDSEL;
    }

    uint32_t cr2 = _usart->CR2;
    if (invert) {
        cr2 |= USART_CR2_RXINV | USART_CR2_TXINV;
    }
    else {
        cr2 &= ~(USART_CR2_RXINV | USART_CR2_TXINV);
    }
    if (swap) {
        cr2 |= USART_CR2_SWAP;
    }
    else {
        cr2 &= ~USART_CR2_SWAP;
    }
    _usart->CR2 = cr2;

    uint32_t cr1 = _usart->CR1;

    if (dma) {
        cr1 |= USART_CR1_RE | USART_CR1_IDLEIE;
    }
    else
    {
        cr1 |= USART_CR1_TE | USART_CR1_TCIE | USART_CR1_RE |
        #if defined(MCU_F051)
            USART_CR1_RXNEIE
        #elif defined(MCU_G071)
            USART_CR1_RXNEIE_RXFNEIE
        #else
        #error
        #endif
            ;
    }
    _usart->CR1 = cr1;

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (_id == CEREAL_ID_USART1)
    {
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_6;
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_0;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    else if (_id == CEREAL_ID_USART2)
    {
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_2;
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (_id == CEREAL_ID_USART_SWCLK)
    {
        GPIO_InitStruct.Pin        = GPIO_PIN_SWCLK;
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // initialize the SWDIO pin as a floating input, which means the user can solder to both pads
        GPIO_InitStruct.Pin        = GPIO_PIN_SWDIO;
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_INPUT;
        GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate  = 0;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
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
        dmacfg.peripheral_inc_enable  = DMA_PERIPHERAL_INC_DISABLE;
        dmacfg.memory_inc_enable      = DMA_MEMORY_INC_ENABLE;
        dmacfg.peripheral_data_width  = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
        dmacfg.memory_data_width      = DMA_MEMORY_DATA_WIDTH_BYTE;
        dmacfg.loop_mode_enable       = 0;
        dmacfg.priority               = DMA_PRIORITY_HIGH;
        dma_init(CEREAL_DMA_CHAN, &dmacfg);

        cereal_dmaRestart();
    }

    usart_enable(_usart, true);

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
