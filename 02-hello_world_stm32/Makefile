PROJECT := hello_world

################################################################################

TOOLCHAIN := arm-none-eabi-

CC := $(TOOLCHAIN)gcc
AR := $(TOOLCHAIN)ar
SIZE := $(TOOLCHAIN)size
OBJCOPY := $(TOOLCHAIN)objcopy

RM := rm
MK := mkdir

PROJECT_DIR := .
OUT_DIR := $(PROJECT_DIR)/out

################################################################################

SRC_FILES := \
	$(PROJECT_DIR)/main.c \
	$(PROJECT_DIR)/hooks.c \
	$(PROJECT_DIR)/system.c \
	$(PROJECT_DIR)/syscalls.c \
	$(PROJECT_DIR)/interrupts.c \
	$(PROJECT_DIR)/startup_stm32f07x.s \
	
INC_DIRS := \
	$(PROJECT_DIR)

LINKER_FILE := $(PROJECT_DIR)/stm32f0_flash.ld

################################################################################

COMMON_SRC_DIR := $(PROJECT_DIR)/../00-common

COMMON_SRC_FILES := \
	$(COMMON_SRC_DIR)/assert.c \
	$(COMMON_SRC_DIR)/print.c \

INC_DIRS += \
	$(COMMON_SRC_DIR) \
	$(COMMON_SRC_DIR)/cmsis/device \
	$(COMMON_SRC_DIR)/cmsis/core \

################################################################################

FREERTOS_SRC_DIR := $(PROJECT_DIR)/../FreeRTOS-Kernel

FREERTOS_SRC_FILES := \
	$(FREERTOS_SRC_DIR)/event_groups.c \
	$(FREERTOS_SRC_DIR)/list.c \
	$(FREERTOS_SRC_DIR)/queue.c \
	$(FREERTOS_SRC_DIR)/stream_buffer.c \
	$(FREERTOS_SRC_DIR)/tasks.c \
	$(FREERTOS_SRC_DIR)/timers.c \

FREERTOS_SRC_FILES += \
	$(FREERTOS_SRC_DIR)/portable/GCC/ARM_CM0/port.c \

FREERTOS_SRC_FILES += \
	$(FREERTOS_SRC_DIR)/portable/MemMang/heap_4.c \

INC_DIRS += \
	$(FREERTOS_SRC_DIR)/include \
	$(FREERTOS_SRC_DIR)/portable/GCC/ARM_CM0 \

################################################################################

MCUFLAGS := -DSTM32F072xB -mthumb -mcpu=cortex-m0

CFLAGS := -std=gnu11 -Os -g --pedantic -Wall -Werror
CFLAGS += $(addprefix -I,$(INC_DIRS))
CFLAGS += $(MCUFLAGS)

LDFLAGS := -g -Wl,--gc-sections,-Map,$(OUT_DIR)/$(PROJECT).map
LDFLAGS += --specs=nano.specs $(MCUFLAGS) -T$(LINKER_FILE)

################################################################################

OBJS := $(addprefix $(OUT_DIR)/main/, $(patsubst $(PROJECT_DIR)/%.c,%.o,$(filter %.c,$(SRC_FILES))))
OBJS += $(addprefix $(OUT_DIR)/main/, $(patsubst $(PROJECT_DIR)/%.s,%.o,$(filter %.s,$(SRC_FILES))))
OBJS += $(addprefix $(OUT_DIR)/common/, $(patsubst $(COMMON_SRC_DIR)/%.c,%.o,$(COMMON_SRC_FILES)))
OBJS += $(addprefix $(OUT_DIR)/freertos/, $(patsubst $(FREERTOS_SRC_DIR)/%.c,%.o,$(FREERTOS_SRC_FILES)))

TARGET = $(OUT_DIR)/$(PROJECT)

.phony: all clean

all: $(TARGET).hex

.PRECIOUS: $(OUT_DIR)/. $(OUT_DIR)%/.

$(OUT_DIR)/.:
	@$(MK) -p $@

$(OUT_DIR)%/.:
	@$(MK) -p $@

.SECONDEXPANSION:

$(TARGET).hex: $(TARGET).elf
	@$(OBJCOPY) -O ihex $^ $@
	@$(SIZE) $^

$(TARGET).elf: $(OBJS)
	@echo "(LD) $@"
	@$(CC) $(LDFLAGS) $^ -o $@

$(OUT_DIR)/main/%.o: $(PROJECT_DIR)/%.c | $$(@D)/.
	@echo "(CC) $@"
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUT_DIR)/main/%.o: $(PROJECT_DIR)/%.s | $$(@D)/.
	@echo "(ASM) $@"
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUT_DIR)/freertos/%.o: $(FREERTOS_SRC_DIR)/%.c | $$(@D)/.
	@echo "(CC) $@"
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUT_DIR)/common/%.o: $(COMMON_SRC_DIR)/%.c | $$(@D)/.
	@echo "(CC) $@"
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@$(RM) -rf $(OUT_DIR)

flash: $(TARGET).hex
	@st-flash --reset --format ihex write $(TARGET).hex
