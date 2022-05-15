OBJS := $(addprefix $(OUT_DIR)/main/, $(patsubst $(PROJECT_DIR)/%.c,%.o,$(filter %.c,$(SRC_FILES))))
OBJS += $(addprefix $(OUT_DIR)/main/, $(patsubst $(PROJECT_DIR)/%.s,%.o,$(filter %.s,$(SRC_FILES))))
OBJS += $(addprefix $(OUT_DIR)/common/, $(patsubst $(COMMON_SRC_DIR)/%.c,%.o,$(filter %.c,$(COMMON_SRC_FILES))))
OBJS += $(addprefix $(OUT_DIR)/common/, $(patsubst $(COMMON_SRC_DIR)/%.s,%.o,$(filter %.s,$(COMMON_SRC_FILES))))
OBJS += $(addprefix $(OUT_DIR)/freertos/, $(patsubst $(FREERTOS_SRC_DIR)/%.c,%.o,$(filter %.c,$(FREERTOS_SRC_FILES))))
OBJS += $(addprefix $(OUT_DIR)/freertos/, $(patsubst $(FREERTOS_SRC_DIR)/%.s,%.o,$(filter %.s,$(FREERTOS_SRC_FILES))))

TARGET := $(OUT_DIR)/$(PROJECT)

ifeq ($(TARGET_ARCH_NAME),linux)

FINAL_TARGETS := $(TARGET)

else ifeq ($(TARGET_ARCH_NAME),stm32f0)

FINAL_TARGETS := $(TARGET).hex

else

$(error Not supported TARGET_ARCH_NAME)

endif
