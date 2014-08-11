#include "queue.h"

#include "error.h"
#include "radio.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


uint32_t queue_init(queue_t * queue)
{
    queue->head = 0;
    queue->tail = 0;

    return SUCCESS;
}

bool queue_is_empty(queue_t * queue)
{
    return queue->head == queue->tail;
}

bool queue_is_full(queue_t * queue)
{
    return ((queue->tail + 2u) % queue->size) == queue->head;
}

uint32_t queue_new(queue_t * queue, uint8_t ** element)
{
    if (queue_is_full(queue))
        return ERROR_NO_MEMORY;

    *element = &queue->elements[queue->tail * queue->element_size];

    return SUCCESS;
}

uint32_t queue_ready(queue_t * queue)
{
    queue->tail = (queue->tail + 1u) % queue->size;
    return SUCCESS;
}

uint32_t queue_add(queue_t * queue, uint8_t * element)
{
    uint32_t err_code;
    uint8_t * new_element;

    err_code = queue_new(queue, &new_element);
    if (err_code == SUCCESS)
    {
        memcpy(new_element, element, queue->element_size);
        queue->tail = (queue->tail + 1u) % queue->size;
    }
    return err_code;
}

uint32_t queue_get(queue_t * queue, uint8_t * element)
{
    uint8_t * element_ptr;
    uint32_t ret_val = queue_get_ptr(queue, &element_ptr);

    if (ret_val == SUCCESS)
    {
        memcpy(element, element_ptr, queue->element_size);
    }
    return ret_val;
}

uint32_t queue_get_ptr(queue_t * queue, uint8_t ** element)
{
    if (queue_is_empty(queue))
        return ERROR_NOT_FOUND;

    *element = &queue->elements[queue->head * queue->element_size];

    queue->head = (queue->head + 1u) % queue->size;

    return SUCCESS;
}
