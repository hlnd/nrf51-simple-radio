#ifndef TX_QUEUE_H_INCLUDED
#define TX_QUEUE_H_INCLUDED

#include <stdbool.h>

#include "radio.h"

void tx_queue_init(void);

uint32_t tx_queue_get(radio_packet_t * packet);

uint32_t tx_queue_add(radio_packet_t * packet);

uint32_t tx_queue_new(radio_packet_t ** packet);

uint32_t tx_queue_get_ptr(radio_packet_t ** packet);

bool tx_queue_is_empty(void);

#endif
