
TARGETS_F421 := REPEAT_DUALDRIVE_F421 \
	GENERIC_F421_PA2_NOLED GENERIC_F421_PA2_RGB GENERIC_F421_PB4_NOLED GENERIC_F421_PB4_RGB \


HAL_FOLDER_F421 := $(SRC_HAL_DIR)/at32f421

MCU_F421 := -mcpu=cortex-m4 -mthumb
LDSCRIPT_F421 := $(HAL_FOLDER_F421)/AT32F421x6_FLASH.ld

SRC_DIR_F421 += \
	$(SRC_HAL_DIR)/at32 \
	$(SRC_HAL_DIR)/mixed \
	$(HAL_FOLDER_F421) \
	$(HAL_FOLDER_F421)/Drivers/src \


CFLAGS_F421 := -Os \
	-I$(SRC_HAL_DIR)/at32 \
	-I$(SRC_HAL_DIR)/mixed \
	-I$(HAL_FOLDER_F421) \
	-I$(HAL_FOLDER_F421)/Drivers/inc \
	-I$(HAL_FOLDER_F421)/Drivers/CMSIS \
	-I$(HAL_FOLDER_F421)/Drivers/CMSIS/cm4 \
	-I$(HAL_FOLDER_F421)/Drivers/CMSIS/cm4/core_support \
	-I$(HAL_FOLDER_F421)/Drivers/CMSIS/cm4/device_support \


CFLAGS_F421 += \
	-DAT32F421K8U7 \
	-DUSE_STDPERIPH_DRIVER \
	-DAGGRESSIVE_BOOT \
	-DENABLE_LED_BLINK \
	-DENABLE_COMPILE_CLI \


SRC_F421_C   := $(foreach dir, $(SRC_DIR_F421), $(wildcard $(dir)/*.c))
SRC_F421_CPP := $(foreach dir, $(SRC_DIR_F421), $(wildcard $(dir)/*.cpp))
SRC_F421_S   := $(foreach dir, $(SRC_DIR_F421), $(wildcard $(dir)/*.s))
