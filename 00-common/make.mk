COMMON_SRC_FILES := \

INC_DIRS += \
	$(COMMON_SRC_DIR) \

include $(COMMON_SRC_DIR)/hal/make.mk
include $(COMMON_SRC_DIR)/system/make.mk
include $(COMMON_SRC_DIR)/arch/make.mk
include $(COMMON_SRC_DIR)/bsp/make.mk
include $(COMMON_SRC_DIR)/system/make.mk
include $(COMMON_SRC_DIR)/utils/make.mk

include $(COMMON_SRC_DIR)/freertos.mk
include $(COMMON_SRC_DIR)/flags.mk
include $(COMMON_SRC_DIR)/build.mk
