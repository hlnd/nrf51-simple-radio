#ifndef PACKET_QUEUE_H_INCLUDED__
#define PACKET_QUEUE_H_INCLUDED__

#include "radio.h"

#include <stdbool.h>

#define PACKET_QUEUE_INIT(QUEUE, SIZE, ELEMENT_SIZE) \
    do { \
        static uint8_t QUEUE_BUFFER[SIZE*ELEMENT_SIZE]; \
        QUEUE.elements = QUEUE_BUFFER; \
        QUEUE.size = SIZE; \
        QUEUE.element_size = ELEMENT_SIZE; \
        packet_queue_init(&QUEUE); \
    } while (0)

typedef struct
{
    uint32_t head;
    uint32_t tail;
    uint8_t size;
    uint8_t element_size;
    uint8_t * elements;
} packet_queue_t;

uint32_t packet_queue_init(packet_queue_t * queue);

bool packet_queue_is_empty(packet_queue_t * queue);

bool packet_queue_is_full(packet_queue_t * queue);

uint32_t packet_queue_new(packet_queue_t * queue, uint8_t ** packet);

uint32_t packet_queue_add(packet_queue_t * queue, uint8_t * packet);

uint32_t packet_queue_get(packet_queue_t * queue, uint8_t * packet);

#endif
