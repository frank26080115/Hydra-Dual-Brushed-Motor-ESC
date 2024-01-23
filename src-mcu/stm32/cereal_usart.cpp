#include "cereal_usart.h"

static fifo_t fifo_rx_1;
static fifo_t fifo_rx_2;
#ifdef ENABLE_CEREAL_TX
static fifo_t fifo_tx_1;
static fifo_t fifo_tx_2;
#endif
static volatile uint32_t last_rx_time_1;
static volatile uint32_t last_rx_time_2;
static volatile bool is_idle_1;
static volatile bool is_idle_2;

void USARTx_IRQHandler(USART_TypeDef* usart, fifo_t* fifo_rx,
#ifdef ENABLE_CEREAL_TX
fifo_t* fifo_tx,
#endif
volatile bool* is_idle, volatile uint32_t* timestamp)
{
    if (LL_USART_IsActiveFlag_RXNE(usart))
    {
        dbg_evntcnt_add(DBGEVNTID_USART_RX);
        uint8_t x = LL_USART_ReceiveData8(usart);
        fifo_push(fifo_rx, x);
        *timestamp = millis();
    }
    #ifdef ENABLE_CEREAL_TX
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
    #endif
    if (LL_USART_IsActiveFlag_IDLE(usart))
    {
        dbg_evntcnt_add(DBGEVNTID_USART_IDLE);
        LL_USART_ClearFlag_IDLE(usart);
        *is_idle = true;
    }
}

#ifdef __cplusplus
extern "C" {
#endif

void USART1_IRQHandler(void)
{
    USARTx_IRQHandler(USART1, &fifo_rx_1,
    #ifdef ENABLE_CEREAL_TX
    &fifo_tx_1,
    #endif
    (volatile bool*)&is_idle_1, (volatile uint32_t*)&last_rx_time_1);
}

void USART2_IRQHandler(void)
{
    USARTx_IRQHandler(USART2, &fifo_rx_2,
    #ifdef ENABLE_CEREAL_TX
    &fifo_tx_2,
    #endif
    (volatile bool*)&is_idle_2, (volatile uint32_t*)&last_rx_time_2);
}

#ifdef __cplusplus
}
#endif

Cereal_USART::Cereal_USART(void)
{

}

void Cereal_USART::init(uint8_t id, uint32_t baud, bool invert, bool halfdup, bool swap)
{
    _id = id;
    if (id == CEREAL_ID_USART1) {
        _usart = USART1;
    }
    else if (id == CEREAL_ID_USART2) {
        _usart = USART2;
    }
    else if (_id == CEREAL_ID_USART_DEBUG) {
        _usart = USART2;
    }
    else if (_id == CEREAL_ID_USART_SWCLK) {
        _usart = USART2;
    }

    if (_id == CEREAL_ID_USART1) {
        fifo_init(&fifo_rx_1, cer_buff_1, CEREAL_BUFFER_SIZE);
        #ifdef ENABLE_CEREAL_TX
        fifo_init(&fifo_tx_1, cer_buff_2, CEREAL_BUFFER_SIZE);
        fifo_tx = &fifo_tx_1;
        #endif
        is_idle_1 = false;
        last_rx_time_1 = 0;
    }
    else if (_id == CEREAL_ID_USART2 || _id == CEREAL_ID_USART_DEBUG || _id == CEREAL_ID_USART_SWCLK) {
        fifo_init(&fifo_rx_2, cer_buff_1, CEREAL_BUFFER_SIZE);
        #ifdef ENABLE_CEREAL_TX
        fifo_init(&fifo_tx_2, cer_buff_2, CEREAL_BUFFER_SIZE);
        fifo_tx = &fifo_tx_2;
        #endif
        is_idle_2 = false;
        last_rx_time_2 = 0;
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

    _usart->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_TCIE | 
        #if defined(MCU_F051)
            USART_CR1_RXNEIE
        #elif defined(MCU_G071)
            LL_USART_CR1_RXNEIE_RXFNEIE
        #endif
        ;

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
    #if defined(STM32F051DISCO) || defined(STM32G071NUCLEO)
    else if (_id == CEREAL_ID_USART_DEBUG)
    {
        #if defined(STM32F051DISCO)
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
        #elif defined(STM32G071NUCLEO)
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
        #endif
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    #endif

    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART2_IRQn);
}

#ifdef ENABLE_CEREAL_TX
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
#endif

uint32_t Cereal_USART::get_last_time(void)
{
    if (_id == 1 || _id == 3) {
        return last_rx_time_1;
    }
    else if (_id == 2) {
        return last_rx_time_2;
    }
    return 0;
}

bool Cereal_USART::get_idle_flag(bool clr)
{
    bool x = false;
    __disable_irq();
    if (_id == 1 || _id == 3) {
        x = is_idle_1;
        if (clr) {
            is_idle_1 = false;
        }
    }
    else if (_id == 2) {
        x = is_idle_2;
        if (clr) {
            is_idle_2 = false;
        }
    }
    __enable_irq();
    return x;
}

#if defined(DEBUG_PRINT)

#ifdef __cplusplus
extern "C" {
#endif

void debug_flush(void) {
    while (fifo_available(&fifo_tx_2)) {
        // do nothing but wait
    }
}

void debug_writechar(char x) {
    fifo_push(&fifo_tx_2, x);
    if (LL_USART_IsActiveFlag_TXE(USART2)) {
        uint8_t y = fifo_pop(&fifo_tx_2);
        LL_USART_TransmitData8(USART2, y);
    }
    if (x == '\n') {
        debug_flush();
    }
}

int debug_writebuff(uint8_t* buf, int len) {
    for (int i = 0; i < len; i++) {
        debug_writechar(buf[i]);
    }
    return len;
}

#ifdef __cplusplus
}
#endif

#endif