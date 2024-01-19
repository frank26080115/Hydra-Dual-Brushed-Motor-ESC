#include "cereal_usart.h"

static fifo_t* fifo_rx_1;
static fifo_t* fifo_tx_1;
static fifo_t* fifo_rx_2;
static fifo_t* fifo_tx_2;
static volatile uint32_t last_rx_time_1;
static volatile uint32_t last_rx_time_2;
static bool is_idle_1;
static bool is_idle_2;

void USARTx_IRQHandler(USART_TypeDef* usart, fifo_t* fifo_rx, fifo_t* fifo_tx, bool* is_idle, uint32_t* timestamp)
{
    if (LL_USART_IsActiveFlag_RXNE(usart))
    {
        uint8_t x = LL_USART_ReceiveData8(usart);
        fifo_push(fifo_rx, x);
        *timestamp = millis();
    }
    if (LL_USART_IsActiveFlag_TC(usart))
    {
        LL_USART_ClearFlag_TC(usart);
        if (fifo_available(fifo_tx))
        {
            uint8_t x = fifo_pop(fifo_tx);
            LL_USART_TransmitData8(usart, x);
        }
    }
    if (LL_USART_IsActiveFlag_IDLE(usart))
    {
        LL_USART_ClearFlag_IDLE(usart);
        *is_idle = true;
    }
}

void USART1_IRQHandler(void)
{
    USARTx_IRQHandler(USART1, fifo_rx_1, fifo_tx_1, &last_rx_time_1);
}

void USART2_IRQHandler(void)
{
    USARTx_IRQHandler(USART2, fifo_rx_2, fifo_tx_2, &last_rx_time_2);
}

Cereal_USART::Cereal_USART(uint8_t id, uint8_t sz)
{
    _id = id;
    if (id == 1) {
        _usart = USART1;
    }
    else if (id == 2) {
        _usart = USART2;
    }
    _buff_size = sz;
}

void Cereal_USART::begin(uint32_t baud, bool invert, bool halfdup, bool swap)
{
    fifo_rx = fifo_init(_buff_size);
    fifo_tx = fifo_init(_buff_size);

    if (_id == 1) {
        fifo_rx_1 = fifo_rx;
        fifo_tx_1 = fifo_tx;
        is_idle_1 = false;
        last_rx_time_1 = 0;
    }
    else if (_id == 2) {
        fifo_rx_2 = fifo_rx;
        fifo_tx_2 = fifo_tx;
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
    _usart->USART2_CR2 = cr2;

    _usart->USART2_CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_TCIE | USART_CR1_RXNEIE;

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (id == 1)
    {
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_6;
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_0;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    else if (id == 2)
    {
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_2;
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
        GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_EnableIRQ(USART2_IRQn);
}

void Cereal_USART::write(uint8_t x)
{
    fifo_push(fifo_tx, x);
    if (LL_USART_IsActiveFlag_TXE(_usart)) {
        uint8_t y = fifo_pop(fifo_tx);
        LL_USART_TransmitData8(usart, y);
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

int16_t Cereal_USART::read(void)
{
    return fifo_pop(fifo_rx);
}

int16_t Cereal_USART::peek(void)
{
    return fifo_peek(fifo_rx);
}

int Cereal_USART::available(void)
{
    return fifo_available(fifo_rx);
}

int Cereal_USART::read(uint8_t* buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        int16_t c = fifo_read(fifo_rx);
        if (c < 0) {
            return i;
        }
        buf[i] = c;
    }
    return i;
}

void Cereal_USART::reset_buffer(void)
{
    fifo_reset(fifo_rx);
}

uint8_t* Cereal_USART::get_buffer(void)
{
    return fifo_rx->buf;
}

uint32_t Cereal_USART::get_last_time(void)
{
    if (_id == 1) {
        return last_rx_time_1;
    }
    else if (_id == 2) {
        return last_rx_time_2;
    }
}

bool Cereal_USART::get_idle_flag(bool clr)
{
    bool x = false;
    __disable_irq();
    if (_id == 1) {
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
