PROJECT := bsp_basic

################################################################################

PROJECT_DIR := .
COMMON_SRC_DIR := $(PROJECT_DIR)/../00-common

################################################################################

TARGET_ARCH_NAME := linux
BOARD_TYPE_NAME := pc_linux

include $(COMMON_SRC_DIR)/config.mk

################################################################################

SRC_FILES := \
	$(PROJECT_DIR)/main.c \

INC_DIRS := \
	$(PROJECT_DIR) \

################################################################################

include $(COMMON_SRC_DIR)/make.mk
include $(COMMON_SRC_DIR)/rules.mk
