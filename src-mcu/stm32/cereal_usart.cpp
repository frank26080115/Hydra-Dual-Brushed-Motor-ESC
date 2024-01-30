#include "cereal_usart.h"

#define CEREAL_DMAx        DMA1
#define CEREAL_DMA_CHAN    LL_DMA_CHANNEL_5
// for MCU_F051, channel 5 is for USART2, but can be used for USART1 if SYSCFG remaps using USART1_RX_DMA_RMP
// for MCU_G071, this doesn't matter since all channels can be mux'ed

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

Cereal_USART::Cereal_USART(void)
{

}

extern uint8_t crsf_inputGuess;

void Cereal_USART::init(uint8_t id, uint32_t baud, bool invert, bool halfdup, bool swap, bool dma)
{
    _id = id;
    if (id == CEREAL_ID_USART_CRSF)
    {
        #if INPUT_PIN == LL_GPIO_PIN_2
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
        #elif INPUT_PIN == LL_GPIO_PIN_4
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
        #if defined(STM32F051DISCO)
        //GPIO_InitStruct.Pin        = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
        #elif defined(STM32G071NUCLEO)
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
        #endif
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
        #if defined(STM32F051DISCO)
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #elif defined(STM32G071NUCLEO)
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #endif
        dbg_cer_tgt = this;
    }
    #endif

    if (dma) {
        use_dma_on = _u;
        #ifdef MCU_G071
        LL_DMA_SetPeriphRequest(CEREAL_DMAx, CEREAL_DMA_CHAN, _u == CEREAL_ID_USART2 ? LL_DMAMUX_REQ_USART2_RX : LL_DMAMUX_REQ_USART1_RX);
        #endif
        #ifdef MCU_F051
        if (_u == CEREAL_ID_USART1) {
            LL_SYSCFG_SetRemapDMA_USART(LL_SYSCFG_USART1RX_RMP_DMA1CH5);
        }
        #endif
        LL_DMA_SetDataTransferDirection(CEREAL_DMAx, CEREAL_DMA_CHAN, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
        LL_DMA_SetChannelPriorityLevel (CEREAL_DMAx, CEREAL_DMA_CHAN, LL_DMA_PRIORITY_HIGH);      // CRSF is the highest priority
        LL_DMA_SetMode                 (CEREAL_DMAx, CEREAL_DMA_CHAN, LL_DMA_MODE_NORMAL);        // stop if overflow
        LL_DMA_SetPeriphIncMode        (CEREAL_DMAx, CEREAL_DMA_CHAN, LL_DMA_PERIPH_NOINCREMENT);
        LL_DMA_SetMemoryIncMode        (CEREAL_DMAx, CEREAL_DMA_CHAN, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetPeriphSize           (CEREAL_DMAx, CEREAL_DMA_CHAN, LL_DMA_PDATAALIGN_BYTE);
        LL_DMA_SetMemorySize           (CEREAL_DMAx, CEREAL_DMA_CHAN, LL_DMA_MDATAALIGN_BYTE);
        LL_DMA_ConfigAddresses         (CEREAL_DMAx, CEREAL_DMA_CHAN,
                                        LL_USART_DMA_GetRegAddr(_usart, LL_USART_DMA_REG_DATA_RECEIVE),
                                        (uint32_t)dma_buff,
                                        LL_DMA_GetDataTransferDirection(CEREAL_DMAx, CEREAL_DMA_CHAN));
        cereal_dmaRestart();
    }

    LL_USART_Enable(_usart);

    IRQn_Type irqn = _u == CEREAL_ID_USART2 ? USART2_IRQn : USART1_IRQn;

    NVIC_SetPriority(irqn, 1);
    NVIC_EnableIRQ(irqn);
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
