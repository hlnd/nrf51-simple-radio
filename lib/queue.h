#ifndef QUEUE_H_INCLUDED__
#define QUEUE_H_INCLUDED__

#include "radio.h"

#include <stdbool.h>

#define QUEUE_INIT(QUEUE, SIZE, ELEMENT_SIZE) \
    do { \
        static uint8_t QUEUE_BUFFER[SIZE*ELEMENT_SIZE]; \
        QUEUE.elements = QUEUE_BUFFER; \
        QUEUE.size = SIZE; \
        QUEUE.element_size = ELEMENT_SIZE; \
        queue_init(&QUEUE); \
    } while (0)

typedef struct
{
    uint32_t head;
    uint32_t tail;
    uint8_t size;
    uint8_t element_size;
    uint8_t * elements;
} queue_t;

uint32_t queue_init(queue_t * queue);

bool queue_is_empty(queue_t * queue);

bool queue_is_full(queue_t * queue);

uint32_t queue_new(queue_t * queue, uint8_t ** packet);

uint32_t queue_add(queue_t * queue, uint8_t * packet);

uint32_t queue_get(queue_t * queue, uint8_t * packet);

uint32_t queue_get_ptr(queue_t * queue, uint8_t ** element);

uint32_t queue_ready(queue_t * queue);

#endif
