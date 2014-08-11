#ifndef RX_QUEUE_H_INCLUDED
#define RX_QUEUE_H_INCLUDED

#include <stdbool.h>

#include "radio.h"

void rx_queue_init(void);

uint32_t rx_queue_get(radio_packet_t * packet);

uint32_t rx_queue_add(radio_packet_t * packet);

uint32_t rx_queue_new(radio_packet_t ** packet);

uint32_t rx_queue_get_ptr(radio_packet_t ** packet);

uint32_t rx_queue_ready(void);

bool rx_queue_is_empty(void);

#endif
