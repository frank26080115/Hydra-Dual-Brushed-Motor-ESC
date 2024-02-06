#include "cereal.h"
#include "stm32_at32_compat.h"

#define CEREAL_DMAx        DMA1
#define CEREAL_DMA_CHAN    DMA1_CHANNEL5

#include "cereal_usart_shared.h"

void Cereal_USART::init(uint8_t id, uint32_t baud, bool halfdup, bool swap, bool dma, bool invert)
{
    sw_init(id);

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

    cached_cr1 = _usart->ctrl1;
    cached_cr2 = _usart->ctrl2;
    cached_cr3 = _usart->ctrl3;

    IRQn_Type irqn = _u == CEREAL_ID_USART2 ? USART2_IRQn : USART1_IRQn;

    NVIC_SetPriority(irqn, 1);
    NVIC_EnableIRQ(irqn);
}
