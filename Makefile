QUIET = 
ARM_SDK_PREFIX = arm-none-eabi-
CC  = $(ARM_SDK_PREFIX)gcc
CPP = $(ARM_SDK_PREFIX)g++
OC  = $(ARM_SDK_PREFIX)objcopy
ECHO = echo
MAKE = make

SRC_APP_DIR = src-app
SRC_HAL_DIR = src-mcu
BIN_DIR     = obj

include src-mcu/f051/makefile-f051.mk
include src-mcu/g071/makefile-g071.mk

# default build, override from command line
MCU_TYPE ?= F051
VERSION_MAJOR = 1
VERSION_EEPROM = 1
IDENTIFIER = FLOORIT

CFLAGS_COMMON  := -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_EEPROM=$(VERSION_EEPROM)
CFLAGS_COMMON  += -I$(SRC_APP_DIR) -I$(SRC_HAL_DIR) -Os -Wall -ffunction-sections
CFLAGS_COMMON  += -D$(TARGET)
LDFLAGS_COMMON := -specs=nano.specs -lc -lm -lnosys -Wl,--gc-sections -Wl,--print-memory-usage

SRC_COMMON := $(foreach dir, $(SRC_APP_DIR), $(wildcard $(dir)/*.s $(dir)/*.c $(dir)/*.cpp))

FIRMWARE_VERSION := V$(VERSION_MAJOR)E$(VERSION_EEPROM)
TARGET_BASENAME   = $(BIN_DIR)/$(IDENTIFIER)_$(TARGET)_$(FIRMWARE_VERSION)

.PHONY : clean all binary f051 g071
all  : $(TARGETS_F051) $(TARGETS_G071)
f051 : $(TARGETS_F051)
g071 : $(TARGETS_G071)

clean :
	rm -rf $(BIN_DIR)/*

binary : $(TARGET_BASENAME).bin
	@$(ECHO) All done

$(TARGETS_F051) :
	@$(MAKE) -s MCU_TYPE=F051 TARGET=$@ binary

$(TARGETS_G071) :
	@$(MAKE) -s MCU_TYPE=G071 TARGET=$@ binary

$(TARGET_BASENAME).elf: SRC     := $(SRC_COMMON) $(SRC_$(MCU_TYPE))
$(TARGET_BASENAME).elf: CFLAGS  := $(MCU_$(MCU_TYPE)) $(CFLAGS_$(MCU_TYPE)) $(CFLAGS_COMMON)
$(TARGET_BASENAME).elf: LDFLAGS := $(LDFLAGS_COMMON) $(LDFLAGS_$(MCU_TYPE)) -T$(LDSCRIPT_$(MCU_TYPE))
$(TARGET_BASENAME).elf: $(SRC)
	@$(ECHO) Compiling $(notdir $@)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(CC) $(CFLAGS) $(LDFLAGS) -MMD -MP -MF $(@:.elf=.d) -o $(@) $(SRC)

$(TARGET_BASENAME).bin: $(TARGET_BASENAME).elf
	@$(ECHO) Generating $(notdir $@)
	$(QUIET)$(OC) -O binary $(<) $@
	$(QUIET)$(OC) $(<) -O ihex $(@:.bin=.hex)
