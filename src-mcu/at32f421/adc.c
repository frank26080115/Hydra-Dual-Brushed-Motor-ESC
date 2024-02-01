#include "sense.h"

#define ADCx         ADC1
#define ADC_DMAx     DMA1
#define ADC_DMA_CHAN DMA1_CHANNEL1

#ifndef CURRENT_ADC_PIN
#define CURRENT_ADC_PIN    GPIO_PINS_6
#endif
#ifndef VOLTAGE_ADC_PIN
#define VOLTAGE_ADC_PIN    GPIO_PINS_3
#endif
#ifndef CURRENT_ADC_PORT
#define CURRENT_ADC_PORT   GPIOA
#endif
#ifndef VOLTAGE_ADC_PORT
#define VOLTAGE_ADC_PORT   GPIOA
#endif

static uint16_t adc_buff[3];

void adc_init()
{
    gpio_mode_QUICK(CURRENT_ADC_PORT, GPIO_MODE_ANALOG, GPIO_PULL_NONE, CURRENT_ADC_PIN);
    gpio_mode_QUICK(VOLTAGE_ADC_PORT, GPIO_MODE_ANALOG, GPIO_PULL_NONE, VOLTAGE_ADC_PIN);

    dma_init_type dmacfg;
    dma_default_para_init(&dmacfg);
    dmacfg.peripheral_base_addr   = (uint32_t)&(ADCx->odt);
    dmacfg.memory_base_addr       = (uint32_t)&adc_buff;
    dmacfg.direction              = DMA_DIR_PERIPHERAL_TO_MEMORY;
    dmacfg.buffer_size            = 3;
    dmacfg.peripheral_inc_enable  = DMA_PERIPHERAL_INC_DISABLE;
    dmacfg.memory_inc_enable      = DMA_MEMORY_INC_ENABLE;
    dmacfg.peripheral_data_width  = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
    dmacfg.memory_data_width      = DMA_MEMORY_DATA_WIDTH_HALFWORD;
    dmacfg.loop_mode_enable       = TRUE;
    dmacfg.priority               = DMA_PRIORITY_LOW;
    dma_init(ADC_DMA_CHAN, &dmacfg);

    crm_adc_clock_div_set(CRM_ADC_DIV_16);
    adc_base_config_type adccfg;
    adc_base_default_para_init(&adccfg);
    adccfg.sequence_mode           = TRUE;
    adccfg.repeat_mode             = TRUE;
    adccfg.data_align              = ADC_RIGHT_ALIGNMENT;
    adccfg.ordinary_channel_length = 3;
    adc_base_config(ADCx, &adccfg);
    adc_ordinary_channel_set(ADCx, ADC_CHANNEL_VOLTAGE, 1, ADC_SAMPLETIME_28_5);
    adc_ordinary_channel_set(ADCx, ADC_CHANNEL_CURRENT, 2, ADC_SAMPLETIME_28_5);
    adc_ordinary_channel_set(ADCx, ADC_CHANNEL_TEMP   , 3, ADC_SAMPLETIME_28_5);

    adc_tempersensor_vintrv_enable(TRUE);
    adc_ordinary_conversion_trigger_set(ADCx, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);

    adc_dma_mode_enable(ADCx, TRUE);

    adc_enable(ADCx, TRUE);
    adc_calibration_init(ADCx);
    while (adc_calibration_init_status_get(ADCx)) {
    }
    adc_calibration_start(ADCx);
    while (adc_calibration_status_get(ADCx)) {
    }

    dma_channel_enable(ADC_DMA_CHAN, TRUE);
    adc_ordinary_software_trigger_enable(ADCx, TRUE);
}

bool adc_task()
{
    bool ret = false;
    bool start_again = false;
    if (dma_flag_get(DMA1_DTERR1_FLAG))
    {
        dma_flag_clear(DMA1_DTERR1_FLAG);
        start_again = true;
    }
    if (dma_flag_get(DMA1_FDT1_FLAG))
    {
        dma_flag_clear(DMA1_FDT1_FLAG);
        dbg_evntcnt_add(DBGEVNTID_ADC);
        start_again = true;

        adc_raw_temperature = adc_buff[2];
        adc_raw_voltage     = adc_buff[1] / 2;
        adc_raw_current     = adc_buff[0];
        ret = true;
    }
    if (start_again)
    {
        adc_ordinary_software_trigger_enable(ADCx, TRUE);
    }
    return ret;
}
