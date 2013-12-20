#ifndef EVT_QUEUE_H_INCLUDED
#define EVT_QUEUE_H_INCLUDED

#include <stdint.h>

#include "radio.h"

uint32_t evt_queue_init(radio_evt_handler_t * evt_handler);

void evt_queue_add(radio_evt_type_t evt);

#endif
