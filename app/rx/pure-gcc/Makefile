# List all source files the application uses.
APPLICATION_SRCS = $(notdir $(wildcard ../*.c))  
APPLICATION_SRCS += nrf_delay.c
APPLICATION_SRCS += radio.c
APPLICATION_SRCS += packet_queue.c

# Use shell to find name of root folder. Possible but horrible to do in make.
PROJECT_NAME = $(shell basename "$(realpath ../)")

DEVICE = NRF51
BOARD = BOARD_PCA10001
SEGGER_SERIAL = 480200976

#USE_SOFTDEVICE = S110

SDK_PATH = $(HOME)/Projects/nrf51/nrf51822/
TEMPLATE_PATH = $(HOME)/Projects/nrf51-pure-gcc-setup/template/

LIBRARY_PATHS = ../../../lib/
SOURCE_PATHS = ../../../lib/

CFLAGS = -g

GDB_PORT_NUMBER = 2330

include $(TEMPLATE_PATH)Makefile
