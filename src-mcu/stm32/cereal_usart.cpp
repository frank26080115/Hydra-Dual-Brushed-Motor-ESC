#include "cereal.h"
#include "stm32_at32_compat.h"

#define CEREAL_DMAx        DMA1
#define CEREAL_DMA_CHAN    LL_DMA_CHANNEL_5
// for MCU_F051, channel 5 is for USART2, but can be used for USART1 if SYSCFG remaps using USART1_RX_DMA_RMP
// for MCU_G071, this doesn't matter since all channels can be mux'ed

#include "cereal_usart_shared.h"

void Cereal_USART::init(uint8_t id, uint32_t baud, bool halfdup, bool swap, bool dma, bool invert)
{
    sw_init(id);

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
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
    if (_id == CEREAL_ID_USART1)
    {
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_6;
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_0;
        LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    else if (_id == CEREAL_ID_USART2)
    {
        GPIO_InitStruct.Pin        = LL_GPIO_PIN_2;
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if (_id == CEREAL_ID_USART_SWCLK)
    {
        GPIO_InitStruct.Pin        = GPIO_PIN_SWCLK;
        GPIO_InitStruct.Alternate  = LL_GPIO_AF_1;
        LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // initialize the SWDIO pin as a floating input, which means the user can solder to both pads
        GPIO_InitStruct.Pin        = GPIO_PIN_SWDIO;
        GPIO_InitStruct.Mode       = LL_GPIO_MODE_INPUT;
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

    #if defined(STMICRO)
    fix_echo |= halfdup; // STM32's USART perpherial exhibits a echo problem while in half-duplex mode, does not affect AT32, https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/1
    #endif

    LL_USART_Enable(_usart);

    cached_cr1 = _usart->CR1;
    cached_cr2 = _usart->CR2;
    cached_cr3 = _usart->CR3;

    IRQn_Type irqn = _u == CEREAL_ID_USART2 ? USART2_IRQn : USART1_IRQn;

    NVIC_SetPriority(irqn, 1);
    NVIC_EnableIRQ(irqn);
}

void Cereal_USART::drain_echo(void)
{
    // STM32's USART perpherial exhibits a echo problem while in half-duplex mode, does not affect AT32
    // https://github.com/frank26080115/Hydra-Dual-Brushed-Motor-ESC/issues/1
    if (fix_echo && echo_time != 0) {
        reset_buffer(); // this disables RX for a brief time after any transmission
        if ((millis() - echo_time) >= 200) {
            echo_time = 0;
        }
    }
}

int16_t Cereal_USART::read(void)
{
    drain_echo();
    return Cereal::read();
}

int16_t Cereal_USART::peek(void)
{
    drain_echo();
    return Cereal::peek();
}

int16_t Cereal_USART::peekAt(int16_t i)
{
    drain_echo();
    return Cereal::peekAt(i);
}

int16_t Cereal_USART::peekTail(void)
{
    drain_echo();
    return Cereal::peekTail();
}

int16_t Cereal_USART::consume(uint16_t x)
{
    drain_echo();
    return Cereal::consume(x);
}

int Cereal_USART::available(void)
{
    drain_echo();
    return Cereal::available();
}

int Cereal_USART::read(uint8_t* buf, int len)
{
    drain_echo();
    return Cereal::read(buf, len);
}

bool Cereal_USART::popUntil(uint8_t x)
{
    drain_echo();
    return Cereal::popUntil(x);
}
