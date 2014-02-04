# List all source files the application uses.
APPLICATION_SRCS = $(notdir $(wildcard ../*.c))  
APPLICATION_SRCS += nrf_delay.c
APPLICATION_SRCS += buttons.c

APPLICATION_LIBS += radio.lib

# Use shell to find name of root folder. Possible but horrible to do in make.
PROJECT_NAME = $(shell basename "$(realpath ../)")

DEVICE = NRF51
BOARD = BOARD_PCA10001
SEGGER_SERIAL = 480200976

#USE_SOFTDEVICE = S110

SDK_PATH ?= $(HOME)/Projects/nrf51/nrf51822/
TEMPLATE_PATH ?= $(HOME)/Projects/nrf51-pure-gcc-setup/template/

LIB_PATH = ../../../lib/

LIBRARY_PATHS = $(LIB_PATH)
LIBS_PATHS = $(LIB_PATH)
SOURCE_PATHS = $(LIB_PATH)

TEST_PATH = ../../../tests/

CFLAGS = -g

GDB_PORT_NUMBER = 2334

include $(TEMPLATE_PATH)Makefile

include $(LIB_PATH)Makefile

check:
	$(MAKE) -C$(TEST_PATH)

