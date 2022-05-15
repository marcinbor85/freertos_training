FREERTOS_SRC_DIR := $(PROJECT_DIR)/../FreeRTOS-Kernel

FREERTOS_SRC_FILES := \
	$(FREERTOS_SRC_DIR)/event_groups.c \
	$(FREERTOS_SRC_DIR)/list.c \
	$(FREERTOS_SRC_DIR)/queue.c \
	$(FREERTOS_SRC_DIR)/stream_buffer.c \
	$(FREERTOS_SRC_DIR)/tasks.c \
	$(FREERTOS_SRC_DIR)/timers.c \

FREERTOS_SRC_FILES += \
	$(FREERTOS_SRC_DIR)/portable/MemMang/heap_4.c \

INC_DIRS += \
	$(FREERTOS_SRC_DIR)/include \

ifeq ($(TARGET_ARCH_NAME),linux)

FREERTOS_SRC_FILES += \
	$(FREERTOS_SRC_DIR)/portable/ThirdParty/GCC/Posix/port.c \
	$(FREERTOS_SRC_DIR)/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c \

	
INC_DIRS += \
	$(FREERTOS_SRC_DIR)/portable/ThirdParty/GCC/Posix \
	$(FREERTOS_SRC_DIR)/portable/ThirdParty/GCC/Posix/utils \

else ifeq ($(TARGET_ARCH_NAME),stm32f0)

FREERTOS_SRC_FILES += \
	$(FREERTOS_SRC_DIR)/portable/GCC/ARM_CM0/port.c \

INC_DIRS += \
	$(FREERTOS_SRC_DIR)/portable/GCC/ARM_CM0 \

else

$(error Not supported TARGET_ARCH_NAME)

endif
