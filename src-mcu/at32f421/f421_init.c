#include "mcu.h"

void system_clock_config(void)
{
    #ifndef AGGRESSIVE_BOOT // disable this section to save code space, as there is a bootloader that has already done all this
    flash_psr_set(FLASH_WAIT_CYCLE_3);
    crm_reset();
    crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);
    while (crm_flag_get(CRM_HICK_STABLE_FLAG) != SET) {
    }
    crm_pll_config(CRM_PLL_SOURCE_HICK, CRM_PLL_MULT_30);
    crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);
    while (crm_flag_get(CRM_PLL_STABLE_FLAG) != SET) {
    }
    crm_ahb_div_set(CRM_AHB_DIV_1);
    crm_apb2_div_set(CRM_APB2_DIV_1);
    crm_apb1_div_set(CRM_APB1_DIV_1);
    crm_auto_step_mode_enable(TRUE);
    crm_sysclk_switch(CRM_SCLK_PLL);
    while (crm_sysclk_switch_status_get() != CRM_SCLK_PLL) {
    }
    crm_auto_step_mode_enable(FALSE);
    system_core_clock_update();
    #else
    system_core_clock = 120000000;
    #endif
    #ifndef DEVELOPMENT_BOARD
    SCB->VTOR = APPLICATION_ADDRESS;
    #endif
}
