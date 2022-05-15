COMMON_SRC_FILES += \
	$(COMMON_SRC_DIR)/bsp/bsp.c \

COMMON_SRC_FILES += \
	$(COMMON_SRC_DIR)/bsp/$(BOARD_TYPE_NAME)/led.c \
	$(COMMON_SRC_DIR)/bsp/$(BOARD_TYPE_NAME)/button.c \
	$(COMMON_SRC_DIR)/bsp/$(BOARD_TYPE_NAME)/uart.c \
