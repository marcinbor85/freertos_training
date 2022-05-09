PROJECT := uart_basic

################################################################################

TOOLCHAIN := 

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
	$(PROJECT_DIR)/main.c

INC_DIRS := \
	$(PROJECT_DIR)

################################################################################

COMMON_SRC_DIR := $(PROJECT_DIR)/../00-common

COMMON_SRC_FILES := \
	$(COMMON_SRC_DIR)/assert.c \
	$(COMMON_SRC_DIR)/print.c \
	$(COMMON_SRC_DIR)/log.c \
	$(COMMON_SRC_DIR)/hal/uart.c \
	$(COMMON_SRC_DIR)/arch/linux/hw_uart.c \

INC_DIRS += \
	$(COMMON_SRC_DIR) \
	$(COMMON_SRC_DIR)/hal \
	$(COMMON_SRC_DIR)/arch \

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
	$(FREERTOS_SRC_DIR)/portable/ThirdParty/GCC/Posix/port.c \
	$(FREERTOS_SRC_DIR)/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c \

FREERTOS_SRC_FILES += \
	$(FREERTOS_SRC_DIR)/portable/MemMang/heap_3.c \

INC_DIRS += \
	$(FREERTOS_SRC_DIR)/include \
	$(FREERTOS_SRC_DIR)/portable/ThirdParty/GCC/Posix \
	$(FREERTOS_SRC_DIR)/portable/ThirdParty/GCC/Posix/utils \

################################################################################

CFLAGS := -std=gnu11 -O0 -g --pedantic -Wall -Werror
CFLAGS += $(addprefix -I,$(INC_DIRS))

LDFLAGS := -g -Wl,--gc-sections,-Map,$(OUT_DIR)/$(PROJECT).map
LDFLAGS += -lpthread -lrt

################################################################################

OBJS := $(addprefix $(OUT_DIR)/main/, $(patsubst $(PROJECT_DIR)/%.c,%.o,$(SRC_FILES)))
OBJS += $(addprefix $(OUT_DIR)/common/, $(patsubst $(COMMON_SRC_DIR)/%.c,%.o,$(COMMON_SRC_FILES)))
OBJS += $(addprefix $(OUT_DIR)/freertos/, $(patsubst $(FREERTOS_SRC_DIR)/%.c,%.o,$(FREERTOS_SRC_FILES)))

TARGET = $(OUT_DIR)/$(PROJECT)

.phony: all clean

all: $(TARGET)

.PRECIOUS: $(OUT_DIR)/. $(OUT_DIR)%/.

$(OUT_DIR)/.:
	@$(MK) -p $@

$(OUT_DIR)%/.:
	@$(MK) -p $@

.SECONDEXPANSION:

$(TARGET): $(OBJS)
	@echo "(LD) $@"
	@$(CC) $(LDFLAGS) $^ -o $@ 
	@$(SIZE) $@

$(OUT_DIR)/main/%.o: $(PROJECT_DIR)/%.c | $$(@D)/.
	@echo "(CC) $@"
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUT_DIR)/freertos/%.o: $(FREERTOS_SRC_DIR)/%.c | $$(@D)/.
	@echo "(CC) $@"
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUT_DIR)/common/%.o: $(COMMON_SRC_DIR)/%.c | $$(@D)/.
	@echo "(CC) $@"
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@$(RM) -rf $(OUT_DIR)
