QUIET = 
ARM_SDK_PREFIX = arm-none-eabi-
CC   = $(ARM_SDK_PREFIX)gcc
CPP  = $(ARM_SDK_PREFIX)g++
OC   = $(ARM_SDK_PREFIX)objcopy
LD   = $(ARM_SDK_PREFIX)ld
SIZE = $(ARM_SDK_PREFIX)size
ECHO = echo
MAKE = make

SRC_APP_DIR = src-app
SRC_HAL_DIR = src-mcu
BIN_DIR     = bin
OBJ_DIR     = obj

include src-mcu/f051/makefile-f051.mk
include src-mcu/g071/makefile-g071.mk
include src-mcu/f051/makefile-f051disco.mk
include src-mcu/g071/makefile-g071nucleo.mk

# default build, override from command line
MCU_TYPE ?= F051
VERSION_MAJOR = 1
VERSION_EEPROM = 1
IDENTIFIER = HYDRA

CFLAGS_COMMON  := -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_EEPROM=$(VERSION_EEPROM)
CFLAGS_COMMON  += -I$(SRC_APP_DIR) -I$(SRC_HAL_DIR) -Wall -ffunction-sections -fdata-sections -fno-exceptions -ffreestanding -flto
CFLAGS_COMMON  += -D$(TARGET)

SRC_COMMON_C   := $(foreach dir, $(SRC_APP_DIR), $(wildcard $(dir)/*.c))
SRC_COMMON_CPP := $(foreach dir, $(SRC_APP_DIR), $(wildcard $(dir)/*.cpp))
SRC_COMMON_S   := $(foreach dir, $(SRC_APP_DIR), $(wildcard $(dir)/*.s))

SRC_COMMON_C   += $(foreach dir, $(SRC_HAL_DIR), $(wildcard $(dir)/*.c))
SRC_COMMON_CPP += $(foreach dir, $(SRC_HAL_DIR), $(wildcard $(dir)/*.cpp))
SRC_COMMON_S   += $(foreach dir, $(SRC_HAL_DIR), $(wildcard $(dir)/*.s))

FIRMWARE_VERSION := V$(VERSION_MAJOR)E$(VERSION_EEPROM)
TARGET_BASENAME   = $(BIN_DIR)/$(IDENTIFIER)_$(TARGET)_$(FIRMWARE_VERSION)

C_OBJS   = $(addsuffix .o,$(addprefix $(OBJ_DIR)/,$(basename $(SRC_COMMON_C))))   $(addsuffix .o,$(addprefix $(OBJ_DIR)/,$(basename $(SRC_$(MCU_TYPE)_C))))
CPP_OBJS = $(addsuffix .o,$(addprefix $(OBJ_DIR)/,$(basename $(SRC_COMMON_CPP)))) $(addsuffix .o,$(addprefix $(OBJ_DIR)/,$(basename $(SRC_$(MCU_TYPE)_CPP))))
ASM_OBJS = $(addsuffix .o,$(addprefix $(OBJ_DIR)/,$(basename $(SRC_COMMON_S))))   $(addsuffix .o,$(addprefix $(OBJ_DIR)/,$(basename $(SRC_$(MCU_TYPE)_S))))
ALL_OBJS = $(ASM_OBJS) $(CPP_OBJS) $(C_OBJS)

.PHONY : clean cleanobjs all binary f051 g071 f051disco g071nucleo
all  : $(TARGETS_F051) $(TARGETS_G071) $(TARGETS_F051DISCO) $(TARGETS_G071NUCLEO)
f051 : $(TARGETS_F051)
g071 : $(TARGETS_G071)
f051disco  : $(TARGETS_F051DISCO)
g071nucleo : $(TARGETS_G071NUCLEO)

clean :
	rm -rf $(BIN_DIR)/* $(OBJ_DIR)/*

cleanobjs:
	rm -rf $(OBJ_DIR)/*

binary : $(TARGET_BASENAME).hex
	@$(ECHO) $@ $< done

$(TARGETS_F051) :
	@$(MAKE) -s MCU_TYPE=F051 TARGET=$@ cleanobjs binary

$(TARGETS_G071) :
	@$(MAKE) -s MCU_TYPE=G071 TARGET=$@ cleanobjs binary

$(TARGETS_F051DISCO) :
	@$(MAKE) -s MCU_TYPE=F051DISCO TARGET=$@ cleanobjs binary

$(TARGETS_G071NUCLEO) :
	@$(MAKE) -s MCU_TYPE=G071NUCLEO TARGET=$@ cleanobjs binary

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
#	@echo %% $(notdir $<)
	$(CPP) $(MCU_$(MCU_TYPE)) $(CFLAGS_$(MCU_TYPE)) $(CFLAGS_COMMON) -std=c++11 -c -o $@ $<

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
#	@echo %% $(notdir $<)
	@$(CC) $(MCU_$(MCU_TYPE)) $(CFLAGS_$(MCU_TYPE)) $(CFLAGS_COMMON) -std=gnu99 -c -o $@ $<

$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
#	@echo %% $(notdir $<)
	@$(CC) -x assembler-with-cpp $(MCU_$(MCU_TYPE)) $(CFLAGS_$(MCU_TYPE)) $(CFLAGS_COMMON) -c -o $@ $<

$(TARGET_BASENAME).elf: $(ALL_OBJS)
	@echo "[LD] $@"
	$(QUIET)mkdir -p $(dir $@)
	@$(CPP) -o $@ $^ -T$(LDSCRIPT_$(MCU_TYPE)) $(MCU_$(MCU_TYPE)) -lm -lc -lnosys --specs=nano.specs -static -Wl,-gc-sections -Wl,--print-memory-usage -Wl,--cref,-Map=$@.map
	$(SIZE) -G $@
	@arm-none-eabi-nm -t d -S --size-sort $@ > $@.size
	@arm-none-eabi-objdump -t $@ >> $@.size
	@arm-none-eabi-objdump -D $@ > $@.lst

$(TARGET_BASENAME).hex: $(TARGET_BASENAME).elf
	@$(ECHO) Generating $(notdir $@)
	$(QUIET)$(OC) --add-section EEPROM=$< -O ihex $< $@
