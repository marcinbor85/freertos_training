ifeq ($(TARGET_ARCH_NAME),linux)

TOOLCHAIN := 

else ifeq ($(TARGET_ARCH_NAME),stm32f0)

TOOLCHAIN := arm-none-eabi-

else

$(error Not supported TARGET_ARCH_NAME)

endif

CC := $(TOOLCHAIN)gcc
AR := $(TOOLCHAIN)ar
SIZE := $(TOOLCHAIN)size
OBJCOPY := $(TOOLCHAIN)objcopy

RM := rm
MK := mkdir

OUT_DIR := $(PROJECT_DIR)/out
