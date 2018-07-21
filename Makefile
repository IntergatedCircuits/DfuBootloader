########+++++++++++++++++++++++++++++++++++++++########
##++----  Makefile for DfuBootloader project   ----++##
########+++++++++++++++++++++++++++++++++++++++########

# TODO move to command line
SERIES = STM32F0
APP_ADDRESS = 0x08002000
APP_SIZE = 24568
TOTAL_ERASE_MS = 480
VID = 0xFFFF
PID = 0xFFFF
TARGET_HEADER = "<stm32f042x6.h>"

# optimization
OPT = -Os
DEBUG = 0

TARGET = DfuBootloader

# Build path
BUILD_DIR = build


##++----  Target configuration  ----++##

ifeq ($(SERIES),STM32L0)
CORE = m0plus
else ifeq ($(SERIES),STM32F0)
CORE = m0
PROGRAM_US = 30
ERASE_MS = 2
else ifeq ($(SERIES),STM32F1)
CORE = m3
else ifeq ($(SERIES),STM32L1)
CORE = m3
else ifeq ($(SERIES),STM32F2)
CORE = m3
else ifeq ($(SERIES),STM32F3)
CORE = m4
else ifeq ($(SERIES),STM32F4)
CORE = m4
else ifeq ($(SERIES),STM32L4)
CORE = m4
else ifeq ($(SERIES),STM32F7)
CORE = m7
else ifeq ($(SERIES),STM32H7)
CORE = m7
endif

ifeq ($(CORE),m0)
	DEVICE_GROUP = stm32_cm0
else
	DEVICE_GROUP = stm32_cmn
endif

# safe default values
ifndef BSP
BSP = BSP_$(SERIES)xx
endif
ifndef VDD_MV
VDD_MV = 2000
endif

C_DEFS =  \
-DFLASH_APP_ADDRESS=$(APP_ADDRESS) \
-DFLASH_APP_SIZE=$(APP_SIZE) \
-DFLASH_BYTE_PROGRAM_TIME_us=$(PROGRAM_US) \
-DUSBD_VID=$(VID) \
-DUSBD_PID=$(PID) \
-DVDD_VALUE_mV=$(VDD_MV) \
-DSTM32_TARGET_HEADER=$(TARGET_HEADER)

ifdef DFUSE
C_DEFS += \
-DUSBD_DFU_ST_EXTENSION=$(DFUSE) \
-DFLASH_ERASE_TIME_ms=$(ERASE_MS) 
else
C_DEFS += \
-DFLASH_TOTAL_ERASE_TIME_ms=$(TOTAL_ERASE_MS)
endif

##++----  Build tool binaries  ----++##
BINPATH = /usr/bin
PREFIX = arm-none-eabi-
CC = $(BINPATH)/$(PREFIX)gcc
AS = $(BINPATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)/$(PREFIX)objcopy
AR = $(BINPATH)/$(PREFIX)ar
SZ = $(BINPATH)/$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S


##++----  MCU config  ----++##
CPU = -mcpu=cortex-$(CORE)
MCU = $(CPU) -mthumb -mfloat-abi=soft


##++----  Assembler  ----++##
AS_DEFS = 
AS_INCLUDES = 
# assembly sources
AS_SOURCES = Main/$(DEVICE_GROUP)_startup.s

# assembler flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections


##++----  Compiler  ----++##
C_STANDARD = -std=gnu11

# C includes
C_INCLUDES =  \
-I$(BSP) \
-IMain \
-IUSBDevice/Device \
-IUSBDevice/Class/DFU \
-ISTM32_XPD/CMSIS/Include \
-IUSBDevice/PDs/STM32_XPD \
-ISTM32_XPD/CMSIS/Device/ST/$(SERIES)xx/Include \
-ISTM32_XPD/$(SERIES)_XPD/inc

# C sources
C_SOURCES =  \
$(wildcard $(BSP)/*.c) \
$(wildcard Main/*.c) \
$(wildcard USBDevice/Device/Src/*.c) \
$(wildcard USBDevice/Class/DFU/Src/*.c) \
$(wildcard STM32_XPD/$(SERIES)_XPD/src/*.c)

# compiler flags
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections $(C_STANDARD)

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"


##++----  Linker  ----++##
# link script
LDSCRIPT = Main/$(DEVICE_GROUP)_flash.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections


##++----  Build the application  ----++##
# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(AS_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(AS_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

##++----  Clean  ----++##
clean:
	-rm -fR .dep $(BUILD_DIR)


##++----  Dependencies  ----++##
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***