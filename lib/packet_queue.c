#include "packet_queue.h"

#include "error.h"
#include "radio.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


uint32_t packet_queue_init(packet_queue_t * queue)
{
    queue->head = 0;
    queue->tail = 0;

    return SUCCESS;
}

bool packet_queue_is_empty(packet_queue_t * queue)
{
    return queue->head == queue->tail;
}

bool packet_queue_is_full(packet_queue_t * queue)
{
    return ((queue->tail + 1) % queue->size) == queue->head;
}

uint32_t packet_queue_new(packet_queue_t * queue, uint8_t ** element)
{
    if (packet_queue_is_full(queue))
        return ERROR_NO_MEMORY;

    *element = &queue->elements[queue->tail * queue->element_size];

    queue->tail = (queue->tail + 1u) % queue->size;

    return SUCCESS;
}

uint32_t packet_queue_add(packet_queue_t * queue, uint8_t * element)
{
    uint32_t err_code;
    uint8_t * new_element;

    err_code = packet_queue_new(queue, &new_element);
    if (err_code != SUCCESS)
        return err_code;

    memcpy(new_element, element, queue->element_size);

    return SUCCESS;
}

uint32_t packet_queue_get(packet_queue_t * queue, uint8_t * element)
{
    if (packet_queue_is_empty(queue))
        return ERROR_NOT_FOUND;

    memcpy(element, &queue->elements[queue->head * queue->element_size], queue->element_size);

    queue->head = (queue->head + 1u) % queue->size;

    return SUCCESS;
}

