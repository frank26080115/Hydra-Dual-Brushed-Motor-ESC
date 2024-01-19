#include "adc.h"
#include "sense.h"

#define ADCx         ADC1
#define ADC_DMAx     DMA1
#define ADC_DMA_CHAN LL_DMA_CHANNEL_1

static uint16_t adc_buff[3];

void adc_init()
{
    LL_ADC_InitTypeDef     ADC_InitStruct     = { 0 };
    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = { 0 };
    LL_GPIO_InitTypeDef    GPIO_InitStruct    = { 0 };

    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;

    GPIO_InitStruct.Pin = CURRENT_ADC_PIN;
    LL_GPIO_Init(CURRENT_ADC_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = VOLTAGE_ADC_PIN;
    LL_GPIO_Init(VOLTAGE_ADC_PORT, &GPIO_InitStruct);

    LL_DMA_SetDataTransferDirection(ADC_DMAx, ADC_DMA_CHAN, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetChannelPriorityLevel (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_PRIORITY_LOW);
    LL_DMA_SetMode                 (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode        (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode        (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize           (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize           (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_MDATAALIGN_HALFWORD);

    LL_ADC_REG_SetSequencerChAdd(ADCx, VOLTAGE_ADC_CHANNEL);
    LL_ADC_REG_SetSequencerChAdd(ADCx, CURRENT_ADC_CHANNEL);
    LL_ADC_REG_SetSequencerChAdd(ADCx, LL_ADC_CHANNEL_TEMPSENSOR);
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADCx), LL_ADC_PATH_INTERNAL_TEMPSENSOR);

    ADC_InitStruct.Clock                = LL_ADC_CLOCK_SYNC_PCLK_DIV4;
    ADC_InitStruct.Resolution           = LL_ADC_RESOLUTION_12B;
    ADC_InitStruct.DataAlignment        = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode         = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADCx, &ADC_InitStruct);
    ADC_REG_InitStruct.TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode   = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer      = LL_ADC_REG_DMA_TRANSFER_LIMITED;
    ADC_REG_InitStruct.Overrun          = LL_ADC_REG_OVR_DATA_PRESERVED;
    LL_ADC_REG_Init(ADCx, &ADC_REG_InitStruct);
    LL_ADC_REG_SetSequencerScanDirection(ADCx, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
    LL_ADC_SetSamplingTimeCommonChannels(ADCx, LL_ADC_SAMPLINGTIME_71CYCLES_5);
    LL_ADC_DisableIT_EOC(ADCx);
    LL_ADC_DisableIT_EOS(ADCx);

    LL_ADC_StartCalibration(ADCx);
    while (LL_ADC_IsCalibrationOnGoing(ADCx) != 0) {
    }

    volatile uint32_t wait_loop_index = (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES >> 1);
    while (wait_loop_index != 0) {
        wait_loop_index--;
    }
    LL_ADC_Enable(ADCx);
    while (LL_ADC_IsActiveFlag_ADRDY(ADCx) == 0) {
        LL_ADC_Enable(ADCx);
    }

    // NVIC_SetPriority(DMA1_Channel1_IRQn, 3);
    // NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    LL_DMA_ConfigAddresses(ADC_DMAx, ADC_DMA_CHAN, LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA), (uint32_t)&adc_buff, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetDataLength(ADC_DMAx, ADC_DMA_CHAN, 3);

    // LL_DMA_EnableIT_TC(ADC_DMAx, ADC_DMA_CHAN);
    // LL_DMA_EnableIT_TE(ADC_DMAx, ADC_DMA_CHAN);
    LL_DMA_EnableChannel(ADC_DMAx, ADC_DMA_CHAN);

    LL_ADC_REG_StartConversion(ADC1);
}

bool adc_task()
{
    if (LL_DMA_IsActiveFlag_TC1(ADC_DMAx))
    {
        LL_DMA_ClearFlag_TC1(ADC_DMAx);

        adc_raw_temperature = adc_buff[2];
        if (VOLTAGE_ADC_PIN > CURRENT_ADC_PIN) {
            adc_raw_voltage = adc_buff[1];
            adc_raw_current = adc_buff[0];
        } else {
            adc_raw_voltage = adc_buff[0];
            adc_raw_current = adc_buff[1];
        }
        return true;
    }
    return false;
}
