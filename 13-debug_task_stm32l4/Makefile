PROJECT := debug_task

################################################################################

PROJECT_DIR := .
COMMON_SRC_DIR := $(PROJECT_DIR)/../00-common

################################################################################

TARGET_ARCH_NAME := stm32l4
BOARD_TYPE_NAME := nucleo_stm32l4

include $(COMMON_SRC_DIR)/config.mk

################################################################################

SRC_FILES := \
	$(PROJECT_DIR)/main.c \
	
INC_DIRS := \
	$(PROJECT_DIR) \

LINKER_FILE := $(PROJECT_DIR)/stm32l4_flash.ld

################################################################################

FINAL_TARGETS := libopencm3

include $(COMMON_SRC_DIR)/make.mk

################################################################################

LIBOPENCM3_DIR := $(PROJECT_DIR)/../libopencm3

CFLAGS += -I$(LIBOPENCM3_DIR)/include
LDFLAGS += -L$(LIBOPENCM3_DIR)/lib -lopencm3_stm32l4

################################################################################

.NOTPARALLEL:

include $(COMMON_SRC_DIR)/rules.mk

libopencm3:
	$(MAKE) -C $(LIBOPENCM3_DIR) TARGETS='stm32/l4'
