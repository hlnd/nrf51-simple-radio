RADIO_SRCS += radio.c
RADIO_SRCS += queue.c
RADIO_SRCS += packet.c
RADIO_SRCS += evt_queue.c
RADIO_SRCS += tx_queue.c
RADIO_SRCS += rx_queue.c

RADIO_OBJS = $(addprefix $(OUTPUT_PATH), $(RADIO_SRCS:.c=.o))
	
RADIO_LIB = radio.lib

$(RADIO_LIB): $(RADIO_OBJS)
	$(AR) cq $@ $^ 
