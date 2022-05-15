ifeq ($(TARGET_ARCH_NAME),linux)

CFLAGS := -std=gnu11 -O0 -g --pedantic -Wall -Werror
CFLAGS += $(addprefix -I,$(INC_DIRS))

LDFLAGS := -g -Wl,--gc-sections,-Map,$(OUT_DIR)/$(PROJECT).map
LDFLAGS += -lpthread -lrt

else ifeq ($(TARGET_ARCH_NAME),stm32f0)

MCUFLAGS := -DSTM32F072xB -mthumb -mcpu=cortex-m0
MCUFLAGS += -DSTM32F0

CFLAGS := -std=gnu11 -Os -g --pedantic -Wall -Werror
CFLAGS += $(addprefix -I,$(INC_DIRS))
CFLAGS += $(MCUFLAGS)
CFLAGS += -DLOGGING_ENABLED=1

LDFLAGS := -g -Wl,--gc-sections,-Map,$(OUT_DIR)/$(PROJECT).map
LDFLAGS += --specs=nano.specs $(MCUFLAGS) -T$(LINKER_FILE)

else

$(error Not supported TARGET_ARCH_NAME)

endif

CFLAGS += -DTARGET_ARCH_NAME=$(TARGET_ARCH_NAME)
CFLAGS += -DBOARD_TYPE_NAME=$(BOARD_TYPE_NAME)
