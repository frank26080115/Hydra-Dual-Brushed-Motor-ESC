TARGETS_GDF350 := LUMENIER_GDF350

HAL_FOLDER_GDF350 := $(SRC_HAL_DIR)/gd32f350

MCU_GDF350 := -mcpu=cortex-m4 -mthumb
LDSCRIPT_GDF350 := $(HAL_FOLDER_GDF350)/STM32F302CBTX_FLASH.ld

SRC_DIR_GDF350 := \
	$(SRC_HAL_DIR)/stm32 \
	$(SRC_HAL_DIR)/mixed \
	$(HAL_FOLDER_GDF350) \
	$(HAL_FOLDER_GDF350)/Drivers/STM32F3xx_HAL_Driver/Src

CFLAGS_GDF350 := -Os \
	-I$(SRC_HAL_DIR)/stm32 \
	-I$(SRC_HAL_DIR)/mixed \
	-I$(HAL_FOLDER_GDF350) \
	-I$(HAL_FOLDER_GDF350)/Drivers/STM32F3xx_HAL_Driver/Inc \
	-I$(HAL_FOLDER_GDF350)/Drivers/CMSIS/Include \
	-I$(HAL_FOLDER_GDF350)/Drivers/CMSIS/Device/ST/STM32F3xx/Include \


CFLAGS_GDF350 += \
	-DHSE_VALUE=8000000 \
	-DSTM32F302xC \
	-DHSE_STARTUP_TIMEOUT=100 \
	-DLSE_STARTUP_TIMEOUT=5000 \
	-DLSE_VALUE=32768 \
	-DVDD_VALUE=3300 \
	-DLSI_VALUE=32000 \
	-DHSI_VALUE=8000000 \
	-DUSE_FULL_LL_DRIVER \
	-DPREFETCH_ENABLE=1 \
	-DENABLE_LED_BLINK \
	-DENABLE_COMPILE_CLI \


SRC_GDF350_C   := $(foreach dir, $(SRC_DIR_GDF350), $(wildcard $(dir)/*.c))
SRC_GDF350_CPP := $(foreach dir, $(SRC_DIR_GDF350), $(wildcard $(dir)/*.cpp))
SRC_GDF350_S   := $(foreach dir, $(SRC_DIR_GDF350), $(wildcard $(dir)/*.s))
