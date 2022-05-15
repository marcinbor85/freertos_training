COMMON_SRC_FILES += \
	$(COMMON_SRC_DIR)/arch/$(TARGET_ARCH_NAME)/hw/uart.c \

COMMON_SRC_FILES += \
	$(COMMON_SRC_DIR)/arch/$(TARGET_ARCH_NAME)/sys/system.c \
	$(COMMON_SRC_DIR)/arch/$(TARGET_ARCH_NAME)/sys/syscalls.c \
	$(COMMON_SRC_DIR)/arch/$(TARGET_ARCH_NAME)/sys/interrupts.c \
	$(COMMON_SRC_DIR)/arch/$(TARGET_ARCH_NAME)/sys/startup_stm32f07x.s \

INC_DIRS += \
	$(COMMON_SRC_DIR)/arch/$(TARGET_ARCH_NAME)/cmsis/device \
	$(COMMON_SRC_DIR)/arch/$(TARGET_ARCH_NAME)/cmsis/core \
