#pragma once

#ifdef GD32DEV_A_E230
    #define TARGET_NAME             "GD32DEV_A_E230"
    #define DEAD_TIME               40
    #define HARDWARE_GROUP_GD_A
    #define VOLTAGE_PIN_PA6
    #define HAS_TELEMETRY_PIN
#endif

#ifdef RHINO40A_E230
    #define TARGET_NAME             "RHINO40A_E230"
    #define DEAD_TIME               50
    #define HARDWARE_GROUP_GD_A
    #define VOLTAGE_PIN_PA6
    #define HAS_TELEMETRY_PIN
    #define LED_PIN_PA15
#endif

#ifdef CM_MINI_E230
    #define TARGET_NAME             "CM Mini V3  "
    #define DEAD_TIME               50
    #define HARDWARE_GROUP_GD_A
    #define VOLTAGE_PIN_PA6
    #define HAS_TELEMETRY_PIN
    #define LED_PIN_PA15
    #define SLOW_RAMP_DOWN
#endif

#ifdef GD32DEV_B_E230
    #define TARGET_NAME             "GD32DEV_B_E230"
    #define DEAD_TIME               50
    #define HARDWARE_GROUP_GD_B
    #define HAS_TELEMETRY_PIN
#endif

/*********************************************************************AT32F421 targets*********************************************/

#ifdef SWAP_PB0_PA7_F421
    #define TARGET_NAME             "SWAP_PB0_PA7"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_AT_045
    #define HARDWARE_GROUP_AT_F
    #define HAS_TELEMETRY_PIN
    #define TELEMETRY_PIN_PA14
    #define PA6_NTC_ONLY
#endif

#ifdef F4A_SINGLE_F421
    #define TARGET_NAME             "F4A_SINGLE_F421"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_AT_540
    #define HARDWARE_GROUP_AT_E
    #define HAS_TELEMETRY_PIN
    #define TELEMETRY_PIN_PA14
    #define PA6_NTC_ONLY
#endif

#ifdef AT32DEV_F421
    #define TARGET_NAME             "AT32DEV_F421"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_AT_B
    #define HARDWARE_GROUP_AT_045
    #define HAS_TELEMETRY_PIN
#endif

#ifdef REPEAT_DUALDRIVE_F421
    #define TARGET_NAME             "REPEAT_DUAL_F421"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_AT_B
    #define HARDWARE_GROUP_AT_045
    #define HAS_TELEMETRY_PIN
    #define DISABLE_LED
#endif

#ifdef GENERIC_F421_PB4_RGB
    #define TARGET_NAME             "GEN_F421_B4_RGB"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_AT_B
    #define HARDWARE_GROUP_AT_045   // only affects comparators, no effect on Hydra
    #define HAS_TELEMETRY_PIN
    #define USE_LED_STRIP
    #define USE_RGB_LED
#endif

#ifdef GENERIC_F421_PA2_RGB
    #define TARGET_NAME             "GEN_F421_A2_RGB"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_AT_C
    #define HARDWARE_GROUP_AT_045   // only affects comparators, no effect on Hydra
    #define HAS_TELEMETRY_PIN
    #define USE_LED_STRIP
    #define USE_RGB_LED
#endif

#ifdef AT32SLOTCAR_F421
    #define FIRMWARE_NAME           "AT32_SC     "
    #define TARGET_NAME             "AT32SLOTCAR_F421"
    #define DEAD_TIME               180
    #define HARDWARE_GROUP_AT_B
    #define HARDWARE_GROUP_AT_045
#endif

#ifdef AT32F421_PB4_054
    #define TARGET_NAME             "AT32F421_PB4_054"
    #define DEAD_TIME               80
    #define HARDWARE_GROUP_AT_B
    #define HARDWARE_GROUP_AT_054
    #define HAS_TELEMETRY_PIN
#endif

#ifdef AT32F421_PB4_405
    #define TARGET_NAME             "AT32F421_PB4_405"
    #define DEAD_TIME               80
    #define HARDWARE_GROUP_AT_B
    #define HARDWARE_GROUP_AT_405
    #define HAS_TELEMETRY_PIN
#endif

#ifdef AT32F421_PB4_540
    #define TARGET_NAME             "AT32F421_PB4_540"
    #define DEAD_TIME               80
    #define HARDWARE_GROUP_AT_B
    #define HARDWARE_GROUP_AT_540
    #define HAS_TELEMETRY_PIN
#endif

#ifdef FOXEER_F421
    #define TARGET_NAME             "FOXEER_F421"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_AT_540
    #define HARDWARE_GROUP_AT_C
    #define HAS_TELEMETRY_PIN
#endif

#ifdef WRAITH32_F421
    #define TARGET_NAME               "WRAITH32"
    #define DEAD_TIME                 60
    #define HARDWARE_GROUP_AT_045
    #define HARDWARE_GROUP_AT_C
    #define HAS_TELEMETRY_PIN
    #define TARGET_VOLTAGE_DIVIDER       74
    #define MILLIVOLT_PER_AMP            45
    #define CURRENT_OFFSET              526 // mv
    #define TARGET_STALL_PROTECTION_INTERVAL 9000
#endif

#ifdef AIKON_55A_F421
    #define TARGET_NAME             "AIKON_55A_F421"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_AT_C
    #define HARDWARE_GROUP_AT_045
    #define HAS_TELEMETRY_PIN
#endif

#ifdef AIKON_SINGLE_F421
    #define TARGET_NAME             "AIKON_SINGLE_F421"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_AT_B
    #define HARDWARE_GROUP_AT_045
    #define HAS_TELEMETRY_PIN
#endif

#ifdef AT32DEV_F415
    #define TARGET_NAME             "AT32DEV_F415"
    #define DEAD_TIME               80
    #define HARDWARE_GROUP_AT_D
    #define HAS_TELEMETRY_PIN
#endif

#ifdef TEKKO32_F415
    #define TARGET_NAME             "TEKKO32_F415"
    #define DEAD_TIME               100
    #define HARDWARE_GROUP_AT_D
    #define HAS_TELEMETRY_PIN
#endif

#ifdef FD6288_F051
    #define FIRMWARE_NAME           "FD6288_PA2  "
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_A
    #define TARGET_VOLTAGE_DIVIDER  65
    #define HAS_TELEMETRY_PIN
#endif

#ifdef EMAXFORMULA_F051
    #define FIRMWARE_NAME           "EMAX FORMULA"
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_A
    #define TARGET_VOLTAGE_DIVIDER  65
    #define DISABLE_LED
#endif

#ifdef IFLIGHT_F051
    #define FIRMWARE_NAME           "IFlight_50A "
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
#endif

#ifdef RANGLEBOXMARS35A_F051
    #define FIRMWARE_NAME           "RANGLEBOXMARS35A"
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
    #define DISABLE_LED
#endif

#ifdef MP6531_F051
    #define FIRMWARE_NAME           "MP6531_F051"
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_C
    #define HAS_TELEMETRY_PIN
#endif

#ifdef RAZOR32_F051
    #define FIRMWARE_NAME           "Razor32     "
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_C
    #define HAS_TELEMETRY_PIN
    #define TELEMETRY_PIN_PA14
#endif

#ifdef TMOTOR55_F051
    #define FIRMWARE_NAME           "T-MOTOR 55A "
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
#endif

#ifdef TMOTOR45_F051
    #define FIRMWARE_NAME           "T-MOTOR 45A "
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_C
    #define HAS_TELEMETRY_PIN
#endif

#ifdef HGLRC_F051
    #define FIRMWARE_NAME           "HGLRC_60A   "
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_A
    #define HAS_TELEMETRY_PIN
#endif

#ifdef SISKIN_F051
    #define FIRMWARE_NAME           "SISKIN_PA2  "
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_A
    #define HAS_TELEMETRY_PIN
#endif

#ifdef DIATONE_F051
    #define FIRMWARE_NAME           "DIATONE GEN "
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_F
    #define HAS_TELEMETRY_PIN
#endif

#ifdef MAMBA_F40PRO_F051
    #define FIRMWARE_NAME           "MAMBA F40PRO"
    #define DEAD_TIME               20
    #define HARDWARE_GROUP_F0_F
    #define HAS_TELEMETRY_PIN
#endif

#ifdef MAMBA_F50PRO_F051
    #define FIRMWARE_NAME           "MAMBA F50PRO"
    #define DEAD_TIME               14
    #define HARDWARE_GROUP_F0_F
    #define HAS_TELEMETRY_PIN
#endif

#ifdef MAMBA_F60PRO_F051
    #define FIRMWARE_NAME           "MAMBA F60PRO"
    #define DEAD_TIME               20
    #define HARDWARE_GROUP_F0_F
    #define HAS_TELEMETRY_PIN
#endif

#ifdef WRAITH32V1_F051
    #define FIRMWARE_NAME           "Wraith_32"
    #define DEAD_TIME               45
    #define TARGET_VOLTAGE_DIVIDER  75
    #define HARDWARE_GROUP_F0_C
    #define HAS_TELEMETRY_PIN
#endif

#ifdef WRAITH32V2_F051
    #define FIRMWARE_NAME           "WRAITH32V2"
    #define DEAD_TIME               60
    #define TARGET_VOLTAGE_DIVIDER  75
    #define MILLIVOLT_PER_AMP       50
    #define CURRENT_OFFSET          600   // millivolts
    #define HARDWARE_GROUP_F0_C
    #define HAS_TELEMETRY_PIN
#endif

#ifdef AIKONSINGLE_F051
    #define FIRMWARE_NAME           "Aikon AK32  "
    #define DEAD_TIME               40
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
#endif

#ifdef CRTEENSY_HILARIESC_F051
    #define FIRMWARE_NAME           "CR HILARIESC"
    #define DEAD_TIME               40
    #define HARDWARE_GROUP_F0_G
    //#define HAS_TELEMETRY_PIN
#endif

#ifdef FLYCOLOR_F051
    #define FIRMWARE_NAME           "Flycolor Gen"
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_A
    #define HAS_TELEMETRY_PIN
    #define VOLTAGE_ADC_CHANNEL     LL_ADC_CHANNEL_6
    #define VOLTAGE_ADC_PIN         LL_GPIO_PIN_6
    #define CURRENT_ADC_CHANNEL     LL_ADC_CHANNEL_3
    #define CURRENT_ADC_PIN         LL_GPIO_PIN_3
#endif

#ifdef ARIA70A_F051
    #define FIRMWARE_NAME           "ARIA70A"
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_A
    #define HAS_TELEMETRY_PIN
    #define VOLTAGE_ADC_CHANNEL     LL_ADC_CHANNEL_6
    #define VOLTAGE_ADC_PIN         LL_GPIO_PIN_6
    #define CURRENT_ADC_CHANNEL     LL_ADC_CHANNEL_3
    #define CURRENT_ADC_PIN         LL_GPIO_PIN_3
    #define USE_RGB_LED
    #define LED_GPIO_RED            GPIOA
    #define LED_PIN_RED             LL_GPIO_PIN_15
    #define LED_GPIO_GREEN          GPIOB
    #define LED_PIN_GREEN           LL_GPIO_PIN_3
    #define LED_GPIO_BLUE           GPIOB
    #define LED_PIN_BLUE            LL_GPIO_PIN_4
    #define LED_IS_OPENDRAIN        true
#endif

#ifdef HVFLYCOLOR_F051
    #define FIRMWARE_NAME           "FLYCOLOR HV "
    #define DEAD_TIME               70
    #define HARDWARE_GROUP_F0_H
    #define HAS_TELEMETRY_PIN
#endif

#ifdef SKYSTARS_F051
    #define FIRMWARE_NAME           "SkyStars K55"
    #define DEAD_TIME               40
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
#endif

#ifdef BLPWR_F051
    #define FIRMWARE_NAME           "BlPwr-ESC"
    #define DEAD_TIME               20
    #define HARDWARE_GROUP_F0_A
    #define MILLIVOLT_PER_AMP       20
    #define CURRENT_OFFSET          0
    #define TARGET_VOLTAGE_DIVIDER  110
    #define HAS_TELEMETRY_PIN
#endif

#ifdef NEUTRONRC_F051
    #define FIRMWARE_NAME           "NeutronRC F0"
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_B
    #define MILLIVOLT_PER_AMP       33
    #define CURRENT_OFFSET          0
    #define TARGET_VOLTAGE_DIVIDER  210
    #define HAS_TELEMETRY_PIN
    #define USE_RGB_LED
#endif

#ifdef AM32REF_F051
    #define FIRMWARE_NAME           "AM32 Ref-ESC"
    #define DEAD_TIME               45
    #define HARDWARE_GROUP_F0_B
    #define MILLIVOLT_PER_AMP       65
    #define CURRENT_OFFSET          0
    #define TARGET_VOLTAGE_DIVIDER  110
    #define VOLTAGE_ADC_CHANNEL     LL_ADC_CHANNEL_6
    #define VOLTAGE_ADC_PIN         LL_GPIO_PIN_6
    #define CURRENT_ADC_CHANNEL     LL_ADC_CHANNEL_3
    #define CURRENT_ADC_PIN         LL_GPIO_PIN_3
    #define HAS_TELEMETRY_PIN
    #define TARGET_STALL_PROTECTION_INTERVAL 9000
#endif

#ifdef RHINO80A_F051
    #define FIRMWARE_NAME           "RHINO80A_F0 "
    #define DEAD_TIME               20
    #define MILLIVOLT_PER_AMP       34
    #define VOLTAGE_ADC_CHANNEL     LL_ADC_CHANNEL_6
    #define VOLTAGE_ADC_PIN         LL_GPIO_PIN_6
    #define CURRENT_ADC_CHANNEL     LL_ADC_CHANNEL_3
    #define CURRENT_ADC_PIN         LL_GPIO_PIN_3
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
#endif

#ifdef AGFRC_V2_F051
    #define FIRMWARE_NAME           "AGFRC V2"
    #define DEAD_TIME               45
    #define TARGET_VOLTAGE_DIVIDER  110
    #define MILLIVOLT_PER_AMP       50
    #define VOLTAGE_ADC_CHANNEL     LL_ADC_CHANNEL_6
    #define VOLTAGE_ADC_PIN         LL_GPIO_PIN_6
    #define CURRENT_ADC_CHANNEL     LL_ADC_CHANNEL_3
    #define CURRENT_ADC_PIN         LL_GPIO_PIN_3
    #define HARDWARE_GROUP_F0_A
    #define HAS_TELEMETRY_PIN
#endif

#ifdef REPEAT_DRIVE_F051
    #define FIRMWARE_NAME           "REPEAT DRIVE"
    #define DEAD_TIME               25
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
    #define VOLTAGE_BASED_RAMP
    #define USE_RGB_LED
    #define LED_PIN_RED             LL_GPIO_PIN_15
    #define LED_GPIO_RED            GPIOA
#endif

#ifdef GENERIC_F051_PA2_1LED
    #define FIRMWARE_NAME           "GEN F051 A2 1L"
    #define DEAD_TIME               25
    #define HARDWARE_GROUP_F0_A
    #define HAS_TELEMETRY_PIN
    #define LED_PIN                 LL_GPIO_PIN_15
    #define LED_GPIO                GPIOA
#endif

#ifdef GENERIC_F051_PB4_1LED
    #define FIRMWARE_NAME           "GEN F051 B4 1L"
    #define DEAD_TIME               25
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
    #define LED_PIN                 LL_GPIO_PIN_15
    #define LED_GPIO                GPIOA
#endif

#ifdef GENERIC_F051_PA2_RGBLED
    #define FIRMWARE_NAME           "GEN F051 A2 RGB"
    #define DEAD_TIME               25
    #define HARDWARE_GROUP_F0_A
    #define HAS_TELEMETRY_PIN
    #define USE_RGB_LED
    #define LED_PIN_RED2            LL_GPIO_PIN_15
    #define LED_GPIO_RED2           GPIOA
#endif

#ifdef GENERIC_F051_PB4_RGBLED
    #define FIRMWARE_NAME           "GEN F051 B4 RGB"
    #define DEAD_TIME               25
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
    #define USE_RGB_LED
    #define LED_PIN_RED2            LL_GPIO_PIN_15
    #define LED_GPIO_RED2           GPIOA
#endif

#ifdef STM32F051DISCO
    #define FIRMWARE_NAME           "STMDISCO"
    #define DEAD_TIME               25
    #define HARDWARE_GROUP_F0_B
    #define HAS_TELEMETRY_PIN
    #define VOLTAGE_BASED_RAMP
    #define DEVELOPMENT_BOARD
    #define LED_PIN                 LL_GPIO_PIN_9
    #define LED_GPIO                GPIOC
    #define LED_IS_OPENDRAIN        false
    #define VOLTAGE_ADC_PIN         LL_GPIO_PIN_5
    #define VOLTAGE_ADC_CHANNEL     LL_ADC_CHANNEL_5
#endif

#ifdef FLASHHOBBY_F051
    #define FIRMWARE_NAME           "FLASHHOBBY  "
    #define DEAD_TIME               30
    #define TARGET_VOLTAGE_DIVIDER  110
    #define MILLIVOLT_PER_AMP       10
    #define CURRENT_OFFSET          1010   // millivolts
    #define HARDWARE_GROUP_F0_A
    #define HAS_TELEMETRY_PIN
#endif

#ifdef  GEN_64K_G071
    #define FIRMWARE_NAME            "G071 64kESC "
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       67
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_A
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
#endif

#ifdef TMOTOR_G071
    #define FIRMWARE_NAME           "T-Motor G071"
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       67
    #define CURRENT_OFFSET          0
    #define TARGET_VOLTAGE_DIVIDER  110
    #define HARDWARE_GROUP_G0_H
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
#endif

#ifdef  SEQURE_G071
    #define FIRMWARE_NAME            "SEQURE G071 "
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       33
    #define CURRENT_OFFSET          0
    #define TARGET_STALL_PROTECTION_INTERVAL 9000
    #define TARGET_VOLTAGE_DIVIDER  210
    #define HARDWARE_GROUP_G0_A
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
    #define CURRENT_ADC_CHANNEL     LL_ADC_CHANNEL_4
    #define CURRENT_ADC_PIN         LL_GPIO_PIN_4
    #define USE_LED_STRIP
#endif

#ifdef HAKRC_2023_G071
    #define FIRMWARE_NAME           "HAKRC G0_23 "
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       45
    #define CURRENT_OFFSET          515
    #define TARGET_VOLTAGE_DIVIDER  110
    #define HARDWARE_GROUP_G0_I
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
    #define USE_LED_STRIP
#endif

#ifdef NEUTRONRC_G071
    #define FIRMWARE_NAME           "NeutronRC G0"
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       33
    #define CURRENT_OFFSET          0
    #define TARGET_VOLTAGE_DIVIDER  210
    #define HARDWARE_GROUP_G0_N
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
    #define USE_LED_STRIP
#endif

#ifdef GENERIC_G071_1LED
    #define FIRMWARE_NAME           "GEN G071 1LED"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_G0_N
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
    #define LED_PIN                 LL_GPIO_PIN_15
    #define LED_GPIO                GPIOA
#endif

#ifdef GENERIC_G071_RGBLED
    #define FIRMWARE_NAME           "GEN G071 RGBLED"
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_G0_N
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
    #define USE_LED_STRIP
    #define USE_RGB_LED
    #define LED_PIN_RED2            LL_GPIO_PIN_15
    #define LED_GPIO_RED2           GPIOA
#endif

#ifdef STM32G071NUCLEO
    #define FIRMWARE_NAME           "NUCLEO"
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       33
    #define CURRENT_OFFSET          0
    #define TARGET_VOLTAGE_DIVIDER  210
    #define HARDWARE_GROUP_G0_N
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
    #define DEVELOPMENT_BOARD
    #define LED_PIN                 LL_GPIO_PIN_5
    #define LED_GPIO                GPIOA
    #define LED_IS_OPENDRAIN        false
    //#define USE_LED_STRIP
#endif

#ifdef AIKON_PRO_G071
    #define FIRMWARE_NAME           "AIKON PRO 50"
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       67
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_G
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
#endif

#ifdef  GEN_128K_G071
    #define FIRMWARE_NAME           "G071 DevESC "
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       15
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_A
    #define HAS_TELEMETRY_PIN
#endif

#ifdef  IFLIGHT_BLITZ_G071
    #define FIRMWARE_NAME           "Blitz 55 G0 "
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       15
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_F
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
#endif

#ifdef  DT120_G071
    #define FIRMWARE_NAME           "G071 120 DT "
    #define DEAD_TIME               120
    #define MILLIVOLT_PER_AMP       15
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_A
    #define HAS_TELEMETRY_PIN
#endif

#ifdef  DT120_64K_G071
    #define FIRMWARE_NAME           "G071 120 64K"
    #define DEAD_TIME               120
    #define MILLIVOLT_PER_AMP       67
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_A
    #define SIXTY_FOUR_KB_MEMORY
#endif

#ifdef  DT160_64K_G071
    #define FIRMWARE_NAME           "G071 160 64K"
    #define DEAD_TIME               210
    #define MILLIVOLT_PER_AMP       67
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_A
    #define SIXTY_FOUR_KB_MEMORY
#endif

#ifdef  PWM_ENABLE_G071
    #define FIRMWARE_NAME           "G071 PWM_EN "
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       20
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_B
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
#endif

#ifdef  OPEN_DRAIN_G071
    #define FIRMWARE_NAME           "G071 TEENSY "
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       20
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_C
    #define HAS_TELEMETRY_PIN
    #define SIXTY_FOUR_KB_MEMORY
#endif

#ifdef  OPEN_DRAIN_B_G071
    #define FIRMWARE_NAME           "G071 TEENSYB"
    #define DEAD_TIME               60
    #define MILLIVOLT_PER_AMP       20
    #define CURRENT_OFFSET          0
    #define HARDWARE_GROUP_G0_D
    #define SIXTY_FOUR_KB_MEMORY
#endif

#ifdef  REF_F031
    #define FIRMWARE_NAME           "Yellow DEV  "
    #define DEAD_TIME               60
    #define HARDWARE_GROUP_F031_A
    #define TARGET_STALL_PROTECTION_INTERVAL 8000
    #define MILLIVOLT_PER_AMP       60
    #define HAS_TELEMETRY_PIN
#endif

/********************************** defaults if not set ***************************/

#ifndef     TARGET_VOLTAGE_DIVIDER
    #define TARGET_VOLTAGE_DIVIDER      110
#endif

#ifndef     SINE_DIVIDER
    #define SINE_DIVIDER                2
#endif

#ifndef     MILLIVOLT_PER_AMP
    #define MILLIVOLT_PER_AMP           20
#endif

#ifndef     CURRENT_OFFSET
    #define CURRENT_OFFSET              0
#endif

/************************************ F051 Hardware Groups ****************************/

#ifdef HARDWARE_GROUP_F0_A

    #define MCU_F051
    #define USE_TIMER_15_CHANNEL_1
    #define INPUT_PIN               LL_GPIO_PIN_2
    #define INPUT_PIN_PORT          GPIOA
    #define MAIN_SIGNAL_PA2
    #define IC_TIMER_CHANNEL        LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER       TIM15
    #define IC_TIMER_POINTER        htim15
    #define INPUT_DMA_CHANNEL       LL_DMA_CHANNEL_5
    #define DMA_HANDLE_TYPE_DEF     hdma_tim15_ch1
    #define IC_DMA_IRQ_NAME         DMA1_Channel4_5_IRQn

    #define PHASE_A_GPIO_LOW        LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW   GPIOB
    #define PHASE_A_GPIO_HIGH       LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH  GPIOA

    #define PHASE_B_GPIO_LOW        LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW   GPIOB
    #define PHASE_B_GPIO_HIGH       LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH  GPIOA

    #define PHASE_C_GPIO_LOW        LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW   GPIOA
    #define PHASE_C_GPIO_HIGH       LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH  GPIOA

    #define PHASE_A_COMP COMP_PA5
    #define PHASE_B_COMP COMP_PA4
    #define PHASE_C_COMP COMP_PA0

#endif

#ifdef HARDWARE_GROUP_F0_B

    #define MCU_F051
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN               LL_GPIO_PIN_4
    #define INPUT_PIN_PORT          GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL        LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER       TIM3
    #define IC_TIMER_POINTER        htim3
    #define INPUT_DMA_CHANNEL       LL_DMA_CHANNEL_4
    #define DMA_HANDLE_TYPE_DEF     hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME         DMA1_Channel4_5_IRQn

    #define PHASE_A_GPIO_LOW        LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW   GPIOB
    #define PHASE_A_GPIO_HIGH       LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH  GPIOA

    #define PHASE_B_GPIO_LOW        LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW   GPIOB
    #define PHASE_B_GPIO_HIGH       LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH  GPIOA

    #define PHASE_C_GPIO_LOW        LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW   GPIOA
    #define PHASE_C_GPIO_HIGH       LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH  GPIOA

    #define PHASE_A_COMP COMP_PA0
    #define PHASE_B_COMP COMP_PA4
    #define PHASE_C_COMP COMP_PA5

#endif

#ifdef  HARDWARE_GROUP_F0_C

    #define MCU_F051
    #define USE_TIMER_15_CHANNEL_1
    #define INPUT_PIN               LL_GPIO_PIN_2
    #define INPUT_PIN_PORT          GPIOA
    #define MAIN_SIGNAL_PA2
    #define IC_TIMER_CHANNEL        LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER       TIM15
    #define IC_TIMER_POINTER        htim15
    #define INPUT_DMA_CHANNEL       LL_DMA_CHANNEL_5
    #define DMA_HANDLE_TYPE_DEF     hdma_tim15_ch1
    #define IC_DMA_IRQ_NAME         DMA1_Channel4_5_IRQn

    #define PHASE_A_GPIO_LOW        LL_GPIO_PIN_0
    #define PHASE_A_GPIO_PORT_LOW   GPIOB
    #define PHASE_A_GPIO_HIGH       LL_GPIO_PIN_9
    #define PHASE_A_GPIO_PORT_HIGH  GPIOA

    #define PHASE_B_GPIO_LOW        LL_GPIO_PIN_7
    #define PHASE_B_GPIO_PORT_LOW   GPIOA
    #define PHASE_B_GPIO_HIGH       LL_GPIO_PIN_8
    #define PHASE_B_GPIO_PORT_HIGH  GPIOA

    #define PHASE_C_GPIO_LOW        LL_GPIO_PIN_1
    #define PHASE_C_GPIO_PORT_LOW   GPIOB
    #define PHASE_C_GPIO_HIGH       LL_GPIO_PIN_10
    #define PHASE_C_GPIO_PORT_HIGH  GPIOA

    #define PHASE_A_COMP COMP_PA4
    #define PHASE_B_COMP COMP_PA5
    #define PHASE_C_COMP COMP_PA0

#endif

#ifdef HARDWARE_GROUP_F0_E

    #define MCU_F051
    #define USE_TIMER_15_CHANNEL_1
    #define INPUT_PIN               LL_GPIO_PIN_2
    #define INPUT_PIN_PORT          GPIOA
    #define MAIN_SIGNAL_PA2
    #define IC_TIMER_CHANNEL        LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER       TIM15
    #define IC_TIMER_POINTER        htim15
    #define INPUT_DMA_CHANNEL       LL_DMA_CHANNEL_5
    #define DMA_HANDLE_TYPE_DEF     hdma_tim15_ch1
    #define IC_DMA_IRQ_NAME         DMA1_Channel4_5_IRQn

    #define PHASE_A_GPIO_LOW        LL_GPIO_PIN_7
    #define PHASE_A_GPIO_PORT_LOW   GPIOA
    #define PHASE_A_GPIO_HIGH       LL_GPIO_PIN_8
    #define PHASE_A_GPIO_PORT_HIGH  GPIOA

    #define PHASE_B_GPIO_LOW        LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW   GPIOB
    #define PHASE_B_GPIO_HIGH       LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH  GPIOA

    #define PHASE_C_GPIO_LOW        LL_GPIO_PIN_1
    #define PHASE_C_GPIO_PORT_LOW   GPIOB
    #define PHASE_C_GPIO_HIGH       LL_GPIO_PIN_10
    #define PHASE_C_GPIO_PORT_HIGH  GPIOA

    #define PHASE_A_COMP COMP_PA0
    #define PHASE_B_COMP COMP_PA4
    #define PHASE_C_COMP COMP_PA5

#endif

#ifdef HARDWARE_GROUP_F0_F

    #define MCU_F051
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN               LL_GPIO_PIN_4
    #define INPUT_PIN_PORT          GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL        LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER       TIM3
    #define IC_TIMER_POINTER        htim3
    #define INPUT_DMA_CHANNEL       LL_DMA_CHANNEL_4
    #define DMA_HANDLE_TYPE_DEF     hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME         DMA1_Channel4_5_IRQn

    #define PHASE_A_GPIO_LOW        LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW   GPIOB
    #define PHASE_A_GPIO_HIGH       LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH  GPIOA

    #define PHASE_B_GPIO_LOW        LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW   GPIOB
    #define PHASE_B_GPIO_HIGH       LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH  GPIOA

    #define PHASE_C_GPIO_LOW        LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW   GPIOA
    #define PHASE_C_GPIO_HIGH       LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH  GPIOA

    #define PHASE_A_COMP COMP_PA5
    #define PHASE_B_COMP COMP_PA0
    #define PHASE_C_COMP COMP_PA4

#endif

#ifdef HARDWARE_GROUP_F0_G

    #define MCU_F051
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN               LL_GPIO_PIN_4
    #define INPUT_PIN_PORT          GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL        LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER       TIM3
    #define IC_TIMER_POINTER        htim3
    #define INPUT_DMA_CHANNEL       LL_DMA_CHANNEL_4
    #define DMA_HANDLE_TYPE_DEF     hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME         DMA1_Channel4_5_IRQn

    #define PHASE_A_GPIO_LOW        LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW   GPIOB
    #define PHASE_A_GPIO_HIGH       LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH  GPIOA

    #define PHASE_B_GPIO_LOW        LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW   GPIOB
    #define PHASE_B_GPIO_HIGH       LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH  GPIOA

    #define PHASE_C_GPIO_LOW        LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW   GPIOA
    #define PHASE_C_GPIO_HIGH       LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH  GPIOA

    #define PHASE_A_COMP COMP_PA4
    #define PHASE_B_COMP COMP_PA0
    #define PHASE_C_COMP COMP_PA5

    #define USE_INVERTED_LOW
    //#define USE_INVERTED_HIGH
    #define USE_OPEN_DRAIN_LOW
    #define USE_OPEN_DRAIN_HIGH

#endif

#ifdef HARDWARE_GROUP_F0_H

    #define MCU_F051
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN               LL_GPIO_PIN_4
    #define INPUT_PIN_PORT          GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL        LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER       TIM3
    #define IC_TIMER_POINTER        htim3
    #define INPUT_DMA_CHANNEL       LL_DMA_CHANNEL_4
    #define DMA_HANDLE_TYPE_DEF     hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME         DMA1_Channel4_5_IRQn

    #define PHASE_A_GPIO_LOW        LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW   GPIOB
    #define PHASE_A_GPIO_HIGH       LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH  GPIOA

    #define PHASE_B_GPIO_LOW        LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW   GPIOB
    #define PHASE_B_GPIO_HIGH       LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH  GPIOA

    #define PHASE_C_GPIO_LOW        LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW   GPIOA
    #define PHASE_C_GPIO_HIGH       LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH  GPIOA

    #define PHASE_A_COMP COMP_PA5
    #define PHASE_B_COMP COMP_PA4
    #define PHASE_C_COMP COMP_PA0

#endif

/************************************* G071 Hardware Groups **********************************/

#ifdef HARDWARE_GROUP_G0_A

    #define MCU_G071
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN                      LL_GPIO_PIN_4
    #define INPUT_PIN_PORT                 GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER              TIM3
    #define IC_TIMER_POINTER               htim3

    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_GPIO_LOW               LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW          GPIOB
    #define PHASE_A_GPIO_HIGH              LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH         GPIOA

    #define PHASE_B_GPIO_LOW               LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW          GPIOB
    #define PHASE_B_GPIO_HIGH              LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH         GPIOA

    #define PHASE_C_GPIO_LOW               LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW          GPIOA
    #define PHASE_C_GPIO_HIGH              LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH         GPIOA

    #define PHASE_A_COMP                   LL_COMP_INPUT_MINUS_IO2  // PB7
    #define PHASE_B_COMP                   LL_COMP_INPUT_MINUS_IO1  // PB3
    #define PHASE_C_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA2

#endif

#ifdef HARDWARE_GROUP_G0_B

    #define MCU_G071
    #define PWM_ENABLE_BRIDGE
    #define USE_TIMER_3_CHANNEL
    #define INPUT_PIN                      LL_GPIO_PIN_4
    #define INPUT_PIN_PORT                 GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER              TIM3
    #define IC_TIMER_POINTER               hti
    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_GPIO_PWM               LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_PWM          GPIOA
    #define PHASE_A_GPIO_ENABLE            LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_ENABLE       GPIOB

    #define PHASE_B_GPIO_PWM               LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_PWM          GPIOA
    #define PHASE_B_GPIO_ENABLE            LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_ENABLE       GPIOB

    #define PHASE_C_GPIO_PWM               LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_PWM          GPIOA
    #define PHASE_C_GPIO_ENABLE            LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_ENABLE       GPIOA

    #define PHASE_A_COMP                   LL_COMP_INPUT_MINUS_IO2  // PB7
    #define PHASE_B_COMP                   LL_COMP_INPUT_MINUS_IO1  // PB3
    #define PHASE_C_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA2

#endif

#ifdef HARDWARE_GROUP_G0_C

    #define MCU_G071
    #define OPEN_DRAIN_PWM
    #define PWM_ENABLE_BRIDGE
    #define USE_TIMER_3_CHANNEL
    #define INPUT_PIN                      LL_GPIO_PIN_4
    #define INPUT_PIN_PORT                 GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER              TIM3
    #define IC_TIMER_POINTER               hti
    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_GPIO_PWM               LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_PWM          GPIOA
    #define PHASE_A_GPIO_ENABLE            LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_ENABLE       GPIOB

    #define PHASE_B_GPIO_PWM               LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_PWM          GPIOA
    #define PHASE_B_GPIO_ENABLE            LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_ENABLE       GPIOB

    #define PHASE_C_GPIO_PWM               LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_PWM          GPIOA
    #define PHASE_C_GPIO_ENABLE            LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_ENABLE       GPIOA

    #define PHASE_A_COMP                   LL_COMP_INPUT_MINUS_IO2  // PB7
    #define PHASE_B_COMP                   LL_COMP_INPUT_MINUS_IO1  // PB3
    #define PHASE_C_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA2

#endif

#ifdef HARDWARE_GROUP_G0_D

    #define MCU_G071
    #define OPEN_DRAIN_PWM
    #define PWM_ENABLE_BRIDGE
    #define USE_TIMER_3_CHANNEL
    #define INPUT_PIN                      LL_GPIO_PIN_4
    #define INPUT_PIN_PORT                 GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER              TIM3
    #define IC_TIMER_POINTER               hti
    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_GPIO_PWM               LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_PWM          GPIOA
    #define PHASE_A_GPIO_ENABLE            LL_GPIO_PIN_15
    #define PHASE_A_GPIO_PORT_ENABLE       GPIOA

    #define PHASE_B_GPIO_PWM               LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_PWM          GPIOA
    #define PHASE_B_GPIO_ENABLE            LL_GPIO_PIN_6
    #define PHASE_B_GPIO_PORT_ENABLE       GPIOC

    #define PHASE_C_GPIO_PWM               LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_PWM          GPIOA
    #define PHASE_C_GPIO_ENABLE            LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_ENABLE       GPIOA

    #define PHASE_A_COMP                   LL_COMP_INPUT_MINUS_IO2  // PB7
    #define PHASE_B_COMP                   LL_COMP_INPUT_MINUS_IO1  // PB3
    #define PHASE_C_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA2
#endif

#ifdef HARDWARE_GROUP_G0_F

    #define MCU_G071
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN                      LL_GPIO_PIN_4
    #define INPUT_PIN_PORT                 GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER              TIM3
    #define IC_TIMER_POINTER               hti
    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_GPIO_LOW               LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW          GPIOB
    #define PHASE_A_GPIO_HIGH              LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH         GPIOA

    #define PHASE_B_GPIO_LOW               LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW          GPIOB
    #define PHASE_B_GPIO_HIGH              LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH         GPIOA

    #define PHASE_C_GPIO_LOW               LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW          GPIOA
    #define PHASE_C_GPIO_HIGH              LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH         GPIOA

    #define PHASE_A_COMP                   LL_COMP_INPUT_MINUS_IO1  // PB3
    #define PHASE_B_COMP                   LL_COMP_INPUT_MINUS_IO2  // PB7
    #define PHASE_C_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA2

#endif

#ifdef HARDWARE_GROUP_G0_G

    #define MCU_G071
    #define N_VARIANT
    #define USE_TIMER_16_CHANNEL_1
    #define INPUT_PIN                      LL_GPIO_PIN_6
    #define INPUT_PIN_PORT                 GPIOA
    #define MAIN_SIGNAL_PA6
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER              TIM16
    #define IC_TIMER_POINTER               htim
    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim16_ch1
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_GPIO_LOW               LL_GPIO_PIN_15
    #define PHASE_A_GPIO_PORT_LOW          GPIOB
    #define PHASE_A_GPIO_HIGH              LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH         GPIOA

    #define PHASE_B_GPIO_LOW               LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW          GPIOB
    #define PHASE_B_GPIO_HIGH              LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH         GPIOA

    #define PHASE_C_GPIO_LOW               LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW          GPIOA
    #define PHASE_C_GPIO_HIGH              LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH         GPIOA

    #define PHASE_A_COMP                   LL_COMP_INPUT_MINUS_IO2  // PB7
    #define PHASE_B_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA2
    #define PHASE_C_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA0

    #define PHASE_A_EXTI_LINE              LL_EXTI_LINE_18
    #define PHASE_A_COMP_NUMBER            COMP2

    #define PHASE_B_EXTI_LINE              LL_EXTI_LINE_18
    #define PHASE_B_COMP_NUMBER            COMP2

    #define PHASE_C_EXTI_LINE              LL_EXTI_LINE_17
    #define PHASE_C_COMP_NUMBER            COMP1

    #define VOLTAGE_ADC_PIN                LL_GPIO_PIN_5
    #define VOLTAGE_ADC_CHANNEL            LL_ADC_CHANNEL_5

    #define CURRENT_ADC_PIN                LL_GPIO_PIN_4
    #define CURRENT_ADC_CHANNEL            LL_ADC_CHANNEL_4

#endif

#ifdef HARDWARE_GROUP_G0_H

    #define MCU_G071
    #define N_VARIANT
    #define USE_TIMER_16_CHANNEL_1
    #define INPUT_PIN                      LL_GPIO_PIN_6
    #define INPUT_PIN_PORT                 GPIOA
    #define MAIN_SIGNAL_PA6
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER              TIM16
    #define IC_TIMER_POINTER               htim
    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim16_ch1
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_B_GPIO_LOW               LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW          GPIOB
    #define PHASE_B_GPIO_HIGH              LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH         GPIOA

    #define PHASE_A_GPIO_LOW               LL_GPIO_PIN_15
    #define PHASE_A_GPIO_PORT_LOW          GPIOB
    #define PHASE_A_GPIO_HIGH              LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH         GPIOA

    #define PHASE_C_GPIO_LOW               LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW          GPIOA
    #define PHASE_C_GPIO_HIGH              LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH         GPIOA

    #define PHASE_B_COMP                   LL_COMP_INPUT_MINUS_IO2  // PB7
    #define PHASE_A_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA2
    #define PHASE_C_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA0

    #define PHASE_A_EXTI_LINE              LL_EXTI_LINE_18
    #define PHASE_A_COMP_NUMBER            COMP2

    #define PHASE_B_EXTI_LINE              LL_EXTI_LINE_18
    #define PHASE_B_COMP_NUMBER            COMP2

    #define PHASE_C_EXTI_LINE              LL_EXTI_LINE_17
    #define PHASE_C_COMP_NUMBER            COMP1

    #define VOLTAGE_ADC_PIN                LL_GPIO_PIN_5
    #define VOLTAGE_ADC_CHANNEL            LL_ADC_CHANNEL_5

    #define CURRENT_ADC_PIN                LL_GPIO_PIN_4
    #define CURRENT_ADC_CHANNEL            LL_ADC_CHANNEL_4

#endif

#ifdef HARDWARE_GROUP_G0_N

    #define MCU_G071
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN                      LL_GPIO_PIN_4
    #define INPUT_PIN_PORT                 GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER              TIM3
    #define IC_TIMER_POINTER               hti
    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_GPIO_LOW               LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW          GPIOB
    #define PHASE_A_GPIO_HIGH              LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH         GPIOA

    #define PHASE_B_GPIO_LOW               LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW          GPIOB
    #define PHASE_B_GPIO_HIGH              LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH         GPIOA

    #define PHASE_C_GPIO_LOW               LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW          GPIOA
    #define PHASE_C_GPIO_HIGH              LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH         GPIOA

    #define PHASE_A_COMP                   LL_COMP_INPUT_MINUS_IO2  // PB7
    #define PHASE_B_COMP                   LL_COMP_INPUT_MINUS_IO1  // PB3
    #define PHASE_C_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA2

    #define VOLTAGE_ADC_PIN                LL_GPIO_PIN_6
    #define VOLTAGE_ADC_CHANNEL            LL_ADC_CHANNEL_6

    #define CURRENT_ADC_PIN                LL_GPIO_PIN_4
    #define CURRENT_ADC_CHANNEL            LL_ADC_CHANNEL_4

#endif

#ifdef HARDWARE_GROUP_G0_I

    #define MCU_G071
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN                      LL_GPIO_PIN_4
    #define INPUT_PIN_PORT                 GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH1
    #define IC_TIMER_REGISTER              TIM3
    #define IC_TIMER_POINTER               hti
    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim3_ch1
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_GPIO_LOW               LL_GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW          GPIOB
    #define PHASE_A_GPIO_HIGH              LL_GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH         GPIOA

    #define PHASE_B_GPIO_LOW               LL_GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW          GPIOB
    #define PHASE_B_GPIO_HIGH              LL_GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH         GPIOA

    #define PHASE_C_GPIO_LOW               LL_GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW          GPIOA
    #define PHASE_C_GPIO_HIGH              LL_GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH         GPIOA

    #define PHASE_A_COMP                   LL_COMP_INPUT_MINUS_IO1  // PB3
    #define PHASE_B_COMP                   LL_COMP_INPUT_MINUS_IO2  // PB7
    #define PHASE_C_COMP                   LL_COMP_INPUT_MINUS_IO3  // PA2

    #define VOLTAGE_ADC_PIN                LL_GPIO_PIN_1
    #define VOLTAGE_ADC_CHANNEL            LL_ADC_CHANNEL_1

    #define CURRENT_ADC_PIN                LL_GPIO_PIN_5
    #define CURRENT_ADC_CHANNEL            LL_ADC_CHANNEL_5

#endif

/************************************ G031 Hardware Groups ************************************************/

#ifdef HARDWARE_GROUP_F031_A

    #define EXTI_TYPE_BAC
    #define USE_TIMER_2_CHANNEL_3
    #define MCU_F031

    #define INPUT_PIN                      LL_GPIO_PIN_2
    #define INPUT_PIN_PORT                 GPIOA
    #define MAIN_SIGNAL_PA2
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH3
    #define IC_TIMER_REGISTER              TIM2
    #define IC_TIMER_POINTER               htim2

    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim2_ch3
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_EXTI_PORT              GPIOF
    #define PHASE_B_EXTI_PORT              GPIOF
    #define PHASE_C_EXTI_PORT              GPIOA

    #define PHASE_A_EXTI_PIN               LL_GPIO_PIN_0
    #define PHASE_B_EXTI_PIN               LL_GPIO_PIN_1
    #define PHASE_C_EXTI_PIN               LL_GPIO_PIN_6

    #define PHASE_A_EXTI_LINE              0
    #define PHASE_B_EXTI_LINE              1
    #define PHASE_C_EXTI_LINE              6

    #define SYSCFG_EXTI_PORTA              LL_SYSCFG_EXTI_PORTF
    #define SYSCFG_EXTI_PORTB              LL_SYSCFG_EXTI_PORTF
    #define SYSCFG_EXTI_PORTC              LL_SYSCFG_EXTI_PORTA

    #define SYSCFG_EXTI_LINEA              LL_SYSCFG_EXTI_LINE0
    #define SYSCFG_EXTI_LINEB              LL_SYSCFG_EXTI_LINE1
    #define SYSCFG_EXTI_LINEC              LL_SYSCFG_EXTI_LINE6

    #define PHASE_A_LL_EXTI_LINE           LL_EXTI_LINE_0
    #define PHASE_B_LL_EXTI_LINE           LL_EXTI_LINE_1
    #define PHASE_C_LL_EXTI_LINE           LL_EXTI_LINE_6

    #define EXTI_IRQ1_NAME                 EXTI0_1_IRQn
    #define EXTI_IRQ2_NAME                 EXTI4_15_IRQn

    #define PHASE_A_GPIO_LOW               LL_GPIO_PIN_14
    #define PHASE_A_GPIO_PORT_LOW          GPIOB
    #define PHASE_A_GPIO_HIGH              LL_GPIO_PIN_9
    #define PHASE_A_GPIO_PORT_HIGH         GPIOA

    #define PHASE_B_GPIO_LOW               LL_GPIO_PIN_13
    #define PHASE_B_GPIO_PORT_LOW          GPIOB
    #define PHASE_B_GPIO_HIGH              LL_GPIO_PIN_8
    #define PHASE_B_GPIO_PORT_HIGH         GPIOA

    #define PHASE_C_GPIO_LOW               LL_GPIO_PIN_15
    #define PHASE_C_GPIO_PORT_LOW          GPIOB
    #define PHASE_C_GPIO_HIGH              LL_GPIO_PIN_10
    #define PHASE_C_GPIO_PORT_HIGH         GPIOA

    #define CURRENT_SENSE_ADC_PIN          LL_GPIO_PIN_5
    #define VOLTAGE_SENSE_ADC_PIN          LL_GPIO_PIN_7

    #define CURRENT_ADC_CHANNEL            LL_ADC_CHANNEL_5
    #define VOLTAGE_ADC_CHANNEL            LL_ADC_CHANNEL_7

#endif

#ifdef HARDWARE_GROUP_F031_B

    #define USE_TIMER_2_CHANNEL_3
    #define INPUT_PIN                      LL_GPIO_PIN_2
    #define MAIN_SIGNAL_PA2
    #define IC_TIMER_CHANNEL               LL_TIM_CHANNEL_CH3
    #define IC_TIMER_REGISTER              TIM2
    #define IC_TIMER_POINTER               htim2

    #define INPUT_DMA_CHANNEL              LL_DMA_CHANNEL_1
    #define DMA_HANDLE_TYPE_DEF            hdma_tim2_ch3
    #define IC_DMA_IRQ_NAME                DMA1_Channel1_IRQn

    #define PHASE_A_EXTI_PORT              GPIOF
    #define PHASE_B_EXTI_PORT              GPIOF
    #define PHASE_C_EXTI_PORT              GPIOA

    #define PHASE_A_EXTI_PIN               LL_GPIO_PIN_0
    #define PHASE_B_EXTI_PIN               LL_GPIO_PIN_1
    #define PHASE_C_EXTI_PIN               LL_GPIO_PIN_5

    #define PHASE_A_EXTI_LINE              0
    #define PHASE_B_EXTI_LINE              1
    #define PHASE_C_EXTI_LINE              5

    #define SYSCFG_EXTI_PORTA              LL_SYSCFG_EXTI_PORTF
    #define SYSCFG_EXTI_PORTB              LL_SYSCFG_EXTI_PORTF
    #define SYSCFG_EXTI_PORTC              LL_SYSCFG_EXTI_PORTA

    #define SYSCFG_EXTI_LINEA              LL_SYSCFG_EXTI_LINE0
    #define SYSCFG_EXTI_LINEB              LL_SYSCFG_EXTI_LINE1
    #define SYSCFG_EXTI_LINEC              LL_SYSCFG_EXTI_LINE5

    #define PHASE_A_LL_EXTI_LINE           LL_EXTI_LINE_0
    #define PHASE_B_LL_EXTI_LINE           LL_EXTI_LINE_1
    #define PHASE_C_LL_EXTI_LINE           LL_EXTI_LINE_5

    #define EXTI_IRQ1_NAME                 EXTI0_1_IRQn
    #define EXTI_IRQ2_NAME                 EXTI4_15_IRQn

    #define PHASE_A_GPIO_LOW               LL_GPIO_PIN_14
    #define PHASE_A_GPIO_PORT_LOW          GPIOB
    #define PHASE_A_GPIO_HIGH              LL_GPIO_PIN_9
    #define PHASE_A_GPIO_PORT_HIGH         GPIOA

    #define PHASE_B_GPIO_LOW               LL_GPIO_PIN_13
    #define PHASE_B_GPIO_PORT_LOW          GPIOB
    #define PHASE_B_GPIO_HIGH              LL_GPIO_PIN_8
    #define PHASE_B_GPIO_PORT_HIGH         GPIOA

    #define PHASE_C_GPIO_LOW               LL_GPIO_PIN_15
    #define PHASE_C_GPIO_PORT_LOW          GPIOB
    #define PHASE_C_GPIO_HIGH              LL_GPIO_PIN_10
    #define PHASE_C_GPIO_PORT_HIGH         GPIOA

    #define CURRENT_SENSE_ADC_PIN         LL_GPIO_PIN_3
    #define VOLTAGE_SENSE_ADC_PIN         LL_GPIO_PIN_4

    #define CURRENT_ADC_CHANNEL           LL_ADC_CHANNEL_3
    #define VOLTAGE_ADC_CHANNEL           LL_ADC_CHANNEL_4

#endif

/***********************************************************************************************************/

#ifdef HARDWARE_GROUP_GD_A

    #define MCU_GDE23
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN               GPIO_PIN_4
    #define INPUT_PIN_PORT          GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL        TIMER_CH_0
    #define IC_TIMER_REGISTER       TIMER2
    #define INPUT_DMA_CHANNEL       DMA_CH3
    #define IC_DMA_IRQ_NAME         DMA_Channel3_4_IRQn

    #define PHASE_A_GPIO_LOW        GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW   GPIOB
    #define PHASE_A_GPIO_HIGH       GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH  GPIOA

    #define PHASE_B_GPIO_LOW        GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW   GPIOB
    #define PHASE_B_GPIO_HIGH       GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH  GPIOA

    #define PHASE_C_GPIO_LOW        GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_LOW   GPIOA
    #define PHASE_C_GPIO_HIGH       GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_HIGH  GPIOA

    #define PHASE_A_COMP            CMP_PA0
    #define PHASE_B_COMP            CMP_PA4
    #define PHASE_C_COMP            CMP_PA5

#endif

#ifdef HARDWARE_GROUP_GD_B

    #define MCU_GDE23
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN                   GPIO_PIN_4
    #define INPUT_PIN_PORT              GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL            TIMER_CH_0
    #define IC_TIMER_REGISTER           TIMER2
    #define INPUT_DMA_CHANNEL           DMA_CH3
    #define IC_DMA_IRQ_NAME             DMA_Channel3_4_IRQn

    #define PHASE_A_GPIO_LOW            GPIO_PIN_1
    #define PHASE_A_GPIO_PORT_LOW       GPIOB
    #define PHASE_A_GPIO_HIGH           GPIO_PIN_10
    #define PHASE_A_GPIO_PORT_HIGH      GPIOA

    #define PHASE_B_GPIO_LOW            GPIO_PIN_0
    #define PHASE_B_GPIO_PORT_LOW       GPIOB
    #define PHASE_B_GPIO_HIGH           GPIO_PIN_9
    #define PHASE_B_GPIO_PORT_HIGH      GPIOA

    #define PHASE_C_GPIO_LOW            GPIO_PIN_8
    #define PHASE_C_GPIO_PORT_LOW       GPIOA
    #define PHASE_C_GPIO_HIGH           GPIO_PIN_7
    #define PHASE_C_GPIO_PORT_HIGH      GPIOA

    #define PHASE_A_COMP                CMP_PA0
    #define PHASE_B_COMP                CMP_PA4
    #define PHASE_C_COMP                CMP_PA5

#endif

#ifdef HARDWARE_GROUP_AT_B

    #define MCU_AT421
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN                   GPIO_PINS_4
    #define INPUT_PIN_SOURCE            GPIO_PINS_SOURCE4
    #define INPUT_PIN_PORT              GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL            TMR_SELECT_CHANNEL_1
    #define IC_TIMER_REGISTER           TMR3
    #define INPUT_DMA_CHANNEL           DMA1_CHANNEL4
    #define IC_DMA_IRQ_NAME             DMA1_Channel5_4_IRQn

    #define PHASE_A_GPIO_LOW            GPIO_PINS_1
    #define PHASE_A_PIN_SOURCE_LOW      GPIO_PINS_SOURCE1
    #define PHASE_A_GPIO_PORT_LOW       GPIOB
    #define PHASE_A_GPIO_HIGH           GPIO_PINS_10
    #define PHASE_A_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE10
    #define PHASE_A_GPIO_PORT_HIGH      GPIOA

    #define PHASE_B_GPIO_LOW            GPIO_PINS_0
    #define PHASE_B_PIN_SOURCE_LOW      GPIO_PINS_SOURCE0
    #define PHASE_B_GPIO_PORT_LOW       GPIOB
    #define PHASE_B_GPIO_HIGH           GPIO_PINS_9
    #define PHASE_B_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE9
    #define PHASE_B_GPIO_PORT_HIGH      GPIOA

    #define PHASE_C_GPIO_LOW            GPIO_PINS_7
    #define PHASE_C_PIN_SOURCE_LOW      GPIO_PINS_SOURCE7
    #define PHASE_C_GPIO_PORT_LOW       GPIOA
    #define PHASE_C_GPIO_HIGH           GPIO_PINS_8
    #define PHASE_C_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE8
    #define PHASE_C_GPIO_PORT_HIGH      GPIOA

#endif

#ifdef HARDWARE_GROUP_AT_C

    #define MCU_AT421
    #define USE_TIMER_15_CHANNEL_1
    #define INPUT_PIN                   GPIO_PINS_2
    #define INPUT_PIN_SOURCE            GPIO_PINS_SOURCE2
    #define INPUT_PIN_PORT              GPIOA
    #define MAIN_SIGNAL_PA2
    #define IC_TIMER_CHANNEL            TMR_SELECT_CHANNEL_1
    #define IC_TIMER_REGISTER           TMR15
    #define INPUT_DMA_CHANNEL           DMA1_CHANNEL5
    #define IC_DMA_IRQ_NAME             DMA1_Channel5_4_IRQn
                                        
    #define PHASE_A_GPIO_LOW            GPIO_PINS_1
    #define PHASE_A_PIN_SOURCE_LOW      GPIO_PINS_SOURCE1
    #define PHASE_A_GPIO_PORT_LOW       GPIOB
    #define PHASE_A_GPIO_HIGH           GPIO_PINS_10
    #define PHASE_A_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE10
    #define PHASE_A_GPIO_PORT_HIGH      GPIOA
                                        
    #define PHASE_B_GPIO_LOW            GPIO_PINS_0
    #define PHASE_B_PIN_SOURCE_LOW      GPIO_PINS_SOURCE0
    #define PHASE_B_GPIO_PORT_LOW       GPIOB
    #define PHASE_B_GPIO_HIGH           GPIO_PINS_9
    #define PHASE_B_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE9
    #define PHASE_B_GPIO_PORT_HIGH      GPIOA
                                        
    #define PHASE_C_GPIO_LOW            GPIO_PINS_7
    #define PHASE_C_PIN_SOURCE_LOW      GPIO_PINS_SOURCE7
    #define PHASE_C_GPIO_PORT_LOW       GPIOA
    #define PHASE_C_GPIO_HIGH           GPIO_PINS_8
    #define PHASE_C_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE8
    #define PHASE_C_GPIO_PORT_HIGH      GPIOA

#endif

#ifdef HARDWARE_GROUP_AT_D

    #define MCU_AT415
    #define USE_TIMER_3_CHANNEL_1
    #define INPUT_PIN                   GPIO_PINS_4
    #define INPUT_PIN_PORT              GPIOB
    #define MAIN_SIGNAL_PB4
    #define IC_TIMER_CHANNEL            TMR_SELECT_CHANNEL_1
    #define IC_TIMER_REGISTER           TMR3
    #define INPUT_DMA_CHANNEL           DMA1_CHANNEL6
    #define IC_DMA_IRQ_NAME             DMA1_Channel6_IRQn
                                        
    #define PHASE_A_GPIO_LOW            GPIO_PINS_1
    #define PHASE_A_PIN_SOURCE_LOW      GPIO_PIN_SOURCE1
    #define PHASE_A_GPIO_PORT_LOW       GPIOB
    #define PHASE_A_GPIO_HIGH           GPIO_PINS_10
    #define PHASE_A_PIN_SOURCE_HIGH     GPIO_PIN_SOURCE10
    #define PHASE_A_GPIO_PORT_HIGH      GPIOA
                                        
    #define PHASE_B_GPIO_LOW            GPIO_PINS_0
    #define PHASE_B_PIN_SOURCE_LOW      GPIO_PIN_SOURCE0
    #define PHASE_B_GPIO_PORT_LOW       GPIOB
    #define PHASE_B_GPIO_HIGH           GPIO_PINS_9
    #define PHASE_B_PIN_SOURCE_HIGH     GPIO_PIN_SOURCE9
    #define PHASE_B_GPIO_PORT_HIGH      GPIOA
                                        
    #define PHASE_C_GPIO_LOW            GPIO_PINS_7
    #define PHASE_C_PIN_SOURCE_LOW      GPIO_PIN_SOURCE7
    #define PHASE_C_GPIO_PORT_LOW       GPIOA
    #define PHASE_C_GPIO_HIGH           GPIO_PINS_8
    #define PHASE_C_PIN_SOURCE_HIGH     GPIO_PIN_SOURCE8
    #define PHASE_C_GPIO_PORT_HIGH      GPIOA

    //#define PHASE_A_COMP COMP_INMInput_IN3  // PA0
    //#define PHASE_B_COMP COMP_INMInput_IN1  // pa4
    //#define PHASE_C_COMP COMP_INMInput_IN2  // pa5

    #define PHASE_A_COMP                0x400000E1            // works for polling mode
    #define PHASE_B_COMP                0x400000C1
    #define PHASE_C_COMP                0x400000D1

#endif

#ifdef HARDWARE_GROUP_AT_E

    #define MCU_AT421
    #define USE_TIMER_15_CHANNEL_1
    #define TELEMETRY_PIN_PA14
    #define USE_PA6_TEMP
    #define INPUT_PIN                   GPIO_PINS_2
    #define INPUT_PIN_SOURCE            GPIO_PINS_SOURCE2
    #define INPUT_PIN_PORT              GPIOA
    #define MAIN_SIGNAL_PA2
    #define IC_TIMER_CHANNEL            TMR_SELECT_CHANNEL_1
    #define IC_TIMER_REGISTER           TMR15
    #define INPUT_DMA_CHANNEL           DMA1_CHANNEL5
    #define IC_DMA_IRQ_NAME             DMA1_Channel5_4_IRQn
                                        
    #define PHASE_A_GPIO_LOW            GPIO_PINS_1
    #define PHASE_A_PIN_SOURCE_LOW      GPIO_PINS_SOURCE1
    #define PHASE_A_GPIO_PORT_LOW       GPIOB
    #define PHASE_A_GPIO_HIGH           GPIO_PINS_10
    #define PHASE_A_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE10
    #define PHASE_A_GPIO_PORT_HIGH      GPIOA
                                        
    #define PHASE_B_GPIO_LOW            GPIO_PINS_0
    #define PHASE_B_PIN_SOURCE_LOW      GPIO_PINS_SOURCE0
    #define PHASE_B_GPIO_PORT_LOW       GPIOB
    #define PHASE_B_GPIO_HIGH           GPIO_PINS_9
    #define PHASE_B_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE9
    #define PHASE_B_GPIO_PORT_HIGH      GPIOA
                                        
    #define PHASE_C_GPIO_LOW            GPIO_PINS_7
    #define PHASE_C_PIN_SOURCE_LOW      GPIO_PINS_SOURCE7
    #define PHASE_C_GPIO_PORT_LOW       GPIOA
    #define PHASE_C_GPIO_HIGH           GPIO_PINS_8
    #define PHASE_C_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE8
    #define PHASE_C_GPIO_PORT_HIGH      GPIOA

#endif

#ifdef HARDWARE_GROUP_AT_F

    #define MCU_AT421
    #define USE_TIMER_15_CHANNEL_1
    //#define TELEMETRY_PIN_PA14
    //#define USE_PA6_TEMP
    #define INPUT_PIN                   GPIO_PINS_2
    #define INPUT_PIN_SOURCE            GPIO_PINS_SOURCE2
    #define INPUT_PIN_PORT              GPIOA
    #define MAIN_SIGNAL_PA2
    #define IC_TIMER_CHANNEL            TMR_SELECT_CHANNEL_1
    #define IC_TIMER_REGISTER           TMR15
    #define INPUT_DMA_CHANNEL           DMA1_CHANNEL5
    #define IC_DMA_IRQ_NAME             DMA1_Channel5_4_IRQn
                                        
    #define PHASE_A_GPIO_LOW            GPIO_PINS_1
    #define PHASE_A_PIN_SOURCE_LOW      GPIO_PINS_SOURCE1
    #define PHASE_A_GPIO_PORT_LOW       GPIOB
    #define PHASE_A_GPIO_HIGH           GPIO_PINS_10
    #define PHASE_A_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE10
    #define PHASE_A_GPIO_PORT_HIGH      GPIOA
                                        
    #define PHASE_B_GPIO_LOW            GPIO_PINS_7
    #define PHASE_B_PIN_SOURCE_LOW      GPIO_PINS_SOURCE7
    #define PHASE_B_GPIO_PORT_LOW       GPIOA
    #define PHASE_B_GPIO_HIGH           GPIO_PINS_9
    #define PHASE_B_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE9
    #define PHASE_B_GPIO_PORT_HIGH      GPIOA
                                        
    #define PHASE_C_GPIO_LOW            GPIO_PINS_0
    #define PHASE_C_PIN_SOURCE_LOW      GPIO_PINS_SOURCE0
    #define PHASE_C_GPIO_PORT_LOW       GPIOB
    #define PHASE_C_GPIO_HIGH           GPIO_PINS_8
    #define PHASE_C_PIN_SOURCE_HIGH     GPIO_PINS_SOURCE8
    #define PHASE_C_GPIO_PORT_HIGH      GPIOA

#endif

#ifdef HARDWARE_GROUP_AT_045
    #define PHASE_A_COMP  0x400000E1       // PA0            // works for polling mode
    #define PHASE_B_COMP  0x400000C1       // PA4
    #define PHASE_C_COMP  0x400000D1       // PA5
#endif
#ifdef HARDWARE_GROUP_AT_504
    #define PHASE_A_COMP  0x400000D1       // PA5            // works for polling mode
    #define PHASE_B_COMP  0x400000E1       // PA0
    #define PHASE_C_COMP  0x400000C1       // PA4
#endif
#ifdef HARDWARE_GROUP_AT_450
    #define PHASE_A_COMP  0x400000C1       // PA4            // works for polling mode
    #define PHASE_B_COMP  0x400000D1       // PA5
    #define PHASE_C_COMP  0x400000E1       // PA0
#endif
#ifdef HARDWARE_GROUP_AT_054
    #define PHASE_A_COMP  0x400000E1       // PA0            // works for polling mode
    #define PHASE_B_COMP  0x400000D1       // PA5
    #define PHASE_C_COMP  0x400000C1       // PA4
#endif
#ifdef HARDWARE_GROUP_AT_405
    #define PHASE_A_COMP  0x400000C1       // PA4            // works for polling mode
    #define PHASE_B_COMP  0x400000E1       // PA0
    #define PHASE_C_COMP  0x400000D1       // PA5
#endif
#ifdef HARDWARE_GROUP_AT_540
    #define PHASE_A_COMP  0x400000D1       // PA5           // works for polling mode
    #define PHASE_B_COMP  0x400000C1       // PA4
    #define PHASE_C_COMP  0x400000E1       // PA0
#endif

#ifdef HARDWARE_GROUP_AT_245
    #define PHASE_A_COMP  0x400000F1       // PA2           // works for polling mode
    #define PHASE_B_COMP  0x400000C1       // PA4
    #define PHASE_C_COMP  0x400000D1       // PA5
#endif

/************************************ MCU COMMON PERIPHERALS **********************************************/

#ifdef MCU_F051
    #define STMICRO
    #define CPU_FREQUENCY_MHZ          48
    #define EEPROM_START_ADD           (uint32_t)0x08007C00
    #define INTERVAL_TIMER             TIM2
    #define TEN_KHZ_TIMER              TIM6
    #define UTILITY_TIMER              TIM17
    #define COM_TIMER                  TIM14
    #define TIM1_AUTORELOAD            1999
    #define PWM_DEFAULT_AUTORELOAD     TIM1_AUTORELOAD
    #define APPLICATION_ADDRESS        0x08001000
    #define MAIN_COMP                  COMP1
    #define EXTI_LINE                  LL_EXTI_LINE_21
    #define TARGET_MIN_BEMF_COUNTS     4
    #define COMPARATOR_IRQ             ADC1_COMP_IRQn
    #define USE_ADC
    #ifndef CURRENT_ADC_PIN
        #define CURRENT_ADC_PIN        LL_GPIO_PIN_6
        #define CURRENT_ADC_CHANNEL    LL_ADC_CHANNEL_6
    #endif
    #ifndef CURRENT_ADC_PORT
        #define CURRENT_ADC_PORT       GPIOA
    #endif
    #ifndef VOLTAGE_ADC_PIN
        #define VOLTAGE_ADC_PIN        LL_GPIO_PIN_3
        #define VOLTAGE_ADC_CHANNEL    LL_ADC_CHANNEL_3
    #endif
    #ifndef VOLTAGE_ADC_PORT
        #define VOLTAGE_ADC_PORT       GPIOA
    #endif
    #ifndef LED_IS_OPENDRAIN
    #define LED_IS_OPENDRAIN true
    #endif
    #ifndef LED_PIN
        #define LED_PIN                LL_GPIO_PIN_15
        #define LED_GPIO               GPIOA
    #endif
    #ifdef USE_RGB_LED
        #ifndef LED_PIN_RED
            #define LED_PIN_RED        LL_GPIO_PIN_8
            #define LED_GPIO_RED       GPIOB
        #endif
        #ifndef LED_PIN_GREEN
            #define LED_PIN_GREEN      LL_GPIO_PIN_5
            #define LED_GPIO_GREEN     GPIOB
        #endif
        #ifndef LED_PIN_BLUE
            #define LED_PIN_BLUE       LL_GPIO_PIN_3
            #define LED_GPIO_BLUE      GPIOB
        #endif
    #endif
#endif

#ifdef MCU_F031
    #define STMICRO
    #define CPU_FREQUENCY_MHZ          48
    #define EEPROM_START_ADD           (uint32_t)0x08007C00
    #define INTERVAL_TIMER             TIM3
    #define TEN_KHZ_TIMER              TIM16
    #define UTILITY_TIMER              TIM17
    #define COM_TIMER                  TIM14
    #define TIM1_AUTORELOAD            1999
    #define APPLICATION_ADDRESS        0x08001000
    #define TARGET_MIN_BEMF_COUNTS     3
    //#define HAS_TELEMETRY_PIN // moved to individual ESCs
    #define USE_ADC

    #ifdef USE_RGB_LED
        #ifndef LED_PIN_RED
            #define LED_PIN_RED        LL_GPIO_PIN_8
            #define LED_GPIO_RED       GPIOB
        #endif
        #ifndef LED_PIN_GREEN
            #define LED_PIN_GREEN      LL_GPIO_PIN_5
            #define LED_GPIO_GREEN     GPIOB
        #endif
        #ifndef LED_PIN_BLUE
            #define LED_PIN_BLUE       LL_GPIO_PIN_3
            #define LED_GPIO_BLUE      GPIOB
        #endif
    #endif
#endif

#ifdef MCU_G071
    #define STMICRO
    #define CPU_FREQUENCY_MHZ        64
    #ifdef  SIXTY_FOUR_KB_MEMORY
        #define EEPROM_START_ADD    (uint32_t)0x0800F800
    #else
        #define EEPROM_START_ADD    (uint32_t)0x0801F800
    #endif
    #define INTERVAL_TIMER          TIM2
    #define TEN_KHZ_TIMER           TIM6
    #define UTILITY_TIMER           TIM17
    #define COM_TIMER               TIM14
    #define TIM1_AUTORELOAD         2667
    #define PWM_DEFAULT_AUTORELOAD  TIM1_AUTORELOAD
    #define APPLICATION_ADDRESS     0x08001000
    #define MAIN_COMP               COMP2
    #define EXTI_LINE               LL_EXTI_LINE_18
    #define TARGET_MIN_BEMF_COUNTS  4
    #define COMPARATOR_IRQ          ADC1_COMP_IRQn
    #define USE_ADC
    #ifndef CURRENT_ADC_CHANNEL
        #define CURRENT_ADC_CHANNEL LL_ADC_CHANNEL_5
    #endif
    #ifndef VOLTAGE_ADC_CHANNEL
        #define VOLTAGE_ADC_CHANNEL LL_ADC_CHANNEL_6
    #endif
    #ifndef CURRENT_ADC_PIN
        #define CURRENT_ADC_PIN     LL_GPIO_PIN_5
    #endif
    #ifndef CURRENT_ADC_PORT
        #define CURRENT_ADC_PORT    GPIOA
    #endif
    #ifndef VOLTAGE_ADC_PIN
        #define VOLTAGE_ADC_PIN     LL_GPIO_PIN_6
    #endif
    #ifndef VOLTAGE_ADC_PORT
        #define VOLTAGE_ADC_PORT    GPIOA
    #endif

    // note: nearly all G071 targets use WS2821

    #ifndef LED_IS_OPENDRAIN
    #define LED_IS_OPENDRAIN true
    #endif
    #ifndef LED_PIN
        #define LED_PIN                LL_GPIO_PIN_15
        #define LED_GPIO               GPIOA
    #endif
    #ifdef USE_RGB_LED
        #ifndef LED_PIN_RED
            #define LED_PIN_RED        LL_GPIO_PIN_8
            #define LED_GPIO_RED       GPIOB
        #endif
        #ifndef LED_PIN_GREEN
            #define LED_PIN_GREEN      LL_GPIO_PIN_5
            #define LED_GPIO_GREEN     GPIOB
        #endif
        #ifndef LED_PIN_BLUE
            #define LED_PIN_BLUE       LL_GPIO_PIN_3
            #define LED_GPIO_BLUE      GPIOB
        #endif
    #endif
#endif

#ifdef MCU_GDE23
    #define GIGADEVICES
    #define CPU_FREQUENCY_MHZ       72

    #define EEPROM_START_ADD        (uint32_t)0x08007C00

    #define INTERVAL_TIMER          TIMER5
    #define TEN_KHZ_TIMER           TIMER13
    #define UTILITY_TIMER           TIMER16
    #define COM_TIMER               TIMER15
    #define TIM1_AUTORELOAD         3000
    #define PWM_DEFAULT_AUTORELOAD  TIM1_AUTORELOAD
    #define APPLICATION_ADDRESS     0x08001000
    #define EXTI_LINE   EXTI_21
    #define TARGET_MIN_BEMF_COUNTS  4
    #define USE_ADC
    #define COMPARATOR_IRQ          ADC_CMP_IRQn
#endif

#ifdef MCU_AT421
    #define ARTERY
    #define CPU_FREQUENCY_MHZ        120
    #define EEPROM_START_ADD         (uint32_t)0x08007C00
    #define INTERVAL_TIMER           TMR6
    #define TEN_KHZ_TIMER            TMR14
    #define UTILITY_TIMER            TMR17
    #define COM_TIMER                TMR16
    #define TIM1_AUTORELOAD          5000
    #define PWM_DEFAULT_AUTORELOAD   TIM1_AUTORELOAD
    #define APPLICATION_ADDRESS      0x08001000
    #define EXTI_LINE                EXINT_LINE_21
    #define TARGET_MIN_BEMF_COUNTS   12
    #define COMPARATOR_IRQ           ADC1_CMP_IRQn
    #define USE_ADC
    #ifndef ADC_CHANNEL_CURRENT
        #define ADC_CHANNEL_CURRENT  ADC_CHANNEL_6
    #endif
    #ifndef ADC_CHANNEL_VOLTAGE
        #define ADC_CHANNEL_VOLTAGE  ADC_CHANNEL_3
    #endif
    #ifndef ADC_CHANNEL_TEMP
        #define ADC_CHANNEL_TEMP     ADC_CHANNEL_16
    #endif

    // note: nearly all AT421 targets use WS2821

    #ifndef LED_IS_OPENDRAIN
    #define LED_IS_OPENDRAIN true
    #endif
    #ifndef LED_PIN
        #define LED_PIN                GPIO_PINS_15
        #define LED_GPIO               GPIOA
    #endif
    #ifdef USE_RGB_LED
        #ifndef LED_PIN_RED
            #define LED_PIN_RED        GPIO_PINS_8
            #define LED_GPIO_RED       GPIOB
        #endif
        #ifndef LED_PIN_GREEN
            #define LED_PIN_GREEN      GPIO_PINS_5
            #define LED_GPIO_GREEN     GPIOB
        #endif
        #ifndef LED_PIN_BLUE
            #define LED_PIN_BLUE       GPIO_PINS_3
            #define LED_GPIO_BLUE      GPIOB
        #endif
    #endif
#endif

#ifdef MCU_AT415
    #define ARTERY
    #define CPU_FREQUENCY_MHZ        144
    #define EEPROM_START_ADD         (uint32_t)0x08007C00
    #define INTERVAL_TIMER           TMR4
    #define TEN_KHZ_TIMER            TMR9
    #define UTILITY_TIMER            TMR10
    #define COM_TIMER                TMR11
    #define TIM1_AUTORELOAD          6000
    #define PWM_DEFAULT_AUTORELOAD   TIM1_AUTORELOAD
    #define APPLICATION_ADDRESS      0x08001000
    #define EXTI_LINE                EXINT_LINE_19
    #define TARGET_MIN_BEMF_COUNTS   8
    #define USE_ADC
    #define COMPARATOR_IRQ           CMP1_IRQn
    #define DSHOT_PRE                95

    #ifndef LED_IS_OPENDRAIN
    #define LED_IS_OPENDRAIN true
    #endif
    #ifndef LED_PIN
        #define LED_PIN                GPIO_PINS_15
        #define LED_GPIO               GPIOA
    #endif
    #ifdef USE_RGB_LED
        #ifndef LED_PIN_RED
            #define LED_PIN_RED        GPIO_PINS_8
            #define LED_GPIO_RED       GPIOB
        #endif
        #ifndef LED_PIN_GREEN
            #define LED_PIN_GREEN      GPIO_PINS_5
            #define LED_GPIO_GREEN     GPIOB
        #endif
        #ifndef LED_PIN_BLUE
            #define LED_PIN_BLUE       GPIO_PINS_3
            #define LED_GPIO_BLUE      GPIOB
        #endif
    #endif
#endif
