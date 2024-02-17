#include "sense.h"

#define ADCx         ADC1
#define ADC_DMAx     DMA1
#define ADC_DMA_CHAN LL_DMA_CHANNEL_2

#define USE_ADC_OVERSAMPLING

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

    LL_DMA_SetPeriphRequest        (ADC_DMAx, ADC_DMA_CHAN, LL_DMAMUX_REQ_ADC1);
    LL_DMA_SetDataTransferDirection(ADC_DMAx, ADC_DMA_CHAN, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetChannelPriorityLevel (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_PRIORITY_LOW);
    LL_DMA_SetMode                 (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode        (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode        (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize           (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_PDATAALIGN_WORD);
    LL_DMA_SetMemorySize           (ADC_DMAx, ADC_DMA_CHAN, LL_DMA_MDATAALIGN_HALFWORD);

    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADCx), LL_ADC_PATH_INTERNAL_TEMPSENSOR);

    ADC_REG_InitStruct.TriggerSource    = LL_ADC_REG_TRIG_SOFTWARE;
    ADC_REG_InitStruct.SequencerLength  = LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    ADC_REG_InitStruct.ContinuousMode   = LL_ADC_REG_CONV_SINGLE;
    ADC_REG_InitStruct.DMATransfer      = LL_ADC_REG_DMA_TRANSFER_LIMITED;
    ADC_REG_InitStruct.Overrun          = LL_ADC_REG_OVR_DATA_PRESERVED;
    LL_ADC_REG_Init(ADCx, &ADC_REG_InitStruct);
    #ifdef USE_ADC_OVERSAMPLING
    LL_ADC_SetOverSamplingScope        (ADCx, LL_ADC_OVS_GRP_REGULAR_CONTINUED);
    LL_ADC_SetOverSamplingDiscont      (ADCx, LL_ADC_OVS_REG_CONT);
    LL_ADC_ConfigOverSamplingRatioShift(ADCx, LL_ADC_OVS_RATIO_256, LL_ADC_OVS_SHIFT_RIGHT_8);
    #else
    LL_ADC_SetOverSamplingScope(ADCx, LL_ADC_OVS_DISABLE);
    #endif
    LL_ADC_SetTriggerFrequencyMode      (ADCx, LL_ADC_CLOCK_FREQ_MODE_LOW);
    LL_ADC_REG_SetSequencerConfigurable (ADCx, LL_ADC_REG_SEQ_CONFIGURABLE);
    LL_ADC_SetClock                     (ADCx, LL_ADC_CLOCK_ASYNC_DIV4);
    LL_ADC_SetSamplingTimeCommonChannels(ADCx, LL_ADC_SAMPLINGTIME_COMMON_1, LL_ADC_SAMPLINGTIME_19CYCLES_5);
    LL_ADC_SetSamplingTimeCommonChannels(ADCx, LL_ADC_SAMPLINGTIME_COMMON_2, LL_ADC_SAMPLINGTIME_160CYCLES_5);
    LL_ADC_DisableIT_EOC(ADCx);
    LL_ADC_DisableIT_EOS(ADCx);

    #ifdef USE_ADC_OVERSAMPLING
    ADC_InitStruct.Resolution    = LL_ADC_RESOLUTION_8B;
    #else
    ADC_InitStruct.Resolution    = LL_ADC_RESOLUTION_12B;
    #endif
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    ADC_InitStruct.LowPowerMode  = LL_ADC_LP_MODE_NONE;
    LL_ADC_Init(ADCx, &ADC_InitStruct);

    LL_ADC_REG_SetSequencerRanks (ADCx, LL_ADC_REG_RANK_1, CURRENT_ADC_CHANNEL);
    LL_ADC_SetChannelSamplingTime(ADCx, CURRENT_ADC_CHANNEL, LL_ADC_SAMPLINGTIME_COMMON_1);
    LL_ADC_REG_SetSequencerRanks (ADCx, LL_ADC_REG_RANK_2, VOLTAGE_ADC_CHANNEL);
    LL_ADC_SetChannelSamplingTime(ADCx, VOLTAGE_ADC_CHANNEL, LL_ADC_SAMPLINGTIME_COMMON_1);
    LL_ADC_REG_SetSequencerRanks (ADCx, LL_ADC_REG_RANK_3, LL_ADC_CHANNEL_TEMPSENSOR);
    LL_ADC_SetChannelSamplingTime(ADCx, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_COMMON_2);

    LL_DMA_ConfigAddresses(ADC_DMAx, ADC_DMA_CHAN, LL_ADC_DMA_GetRegAddr(ADCx, LL_ADC_DMA_REG_REGULAR_DATA), (uint32_t)&adc_buff, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetDataLength  (ADC_DMAx, ADC_DMA_CHAN, 3);
    //LL_DMA_EnableIT_TC    (ADC_DMAx, ADC_DMA_CHAN);
    //LL_DMA_EnableIT_TE    (ADC_DMAx, ADC_DMA_CHAN);
    LL_DMA_EnableChannel  (ADC_DMAx, ADC_DMA_CHAN);

    __IO uint32_t wait_loop_index = 0U;
    __IO uint32_t backup_setting_adc_dma_transfer = 0U;

    LL_ADC_EnableInternalRegulator(ADCx);
    wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while (wait_loop_index != 0) {
        wait_loop_index--;
    }
    backup_setting_adc_dma_transfer = LL_ADC_REG_GetDMATransfer(ADCx);
    LL_ADC_REG_SetDMATransfer(ADCx, LL_ADC_REG_DMA_TRANSFER_NONE);

    LL_ADC_StartCalibration(ADCx);

    while (LL_ADC_IsCalibrationOnGoing(ADCx) != 0) {
    }

    LL_ADC_REG_SetDMATransfer(ADCx, backup_setting_adc_dma_transfer);

    wait_loop_index = (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES >> 1);
    while (wait_loop_index != 0) {
        wait_loop_index--;
    }

    LL_ADC_Enable(ADCx);
    while (LL_ADC_IsActiveFlag_ADRDY(ADCx) == 0) {
        LL_ADC_Enable(ADCx);
    }
    ADC->CCR |= ADC_CCR_TSEN;

    LL_ADC_REG_StartConversion(ADCx);
}

bool adc_task()
{
    bool ret = false;
    bool start_again = false;
    if (LL_DMA_IsActiveFlag_TE2(ADC_DMAx))
    {
        LL_DMA_ClearFlag_TE2(ADC_DMAx);
        start_again = true;
    }
    if (LL_DMA_IsActiveFlag_TC2(ADC_DMAx))
    {
        LL_DMA_ClearFlag_TC2(ADC_DMAx);
        dbg_evntcnt_add(DBGEVNTID_ADC);
        start_again = true;

        adc_raw_temperature = adc_buff[2];
        adc_raw_voltage     = adc_buff[1];
        adc_raw_current     = adc_buff[0];

        ret = true;
    }
    if (start_again)
    {
        LL_ADC_REG_StartConversion(ADCx);
    }
    return ret;
}
