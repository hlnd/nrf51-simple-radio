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
    return (queue->tail - queue->head) == (PACKET_QUEUE_SIZE - 1);
}

uint32_t packet_queue_new(packet_queue_t * queue, radio_packet_t ** packet)
{
    if (packet_queue_is_full(queue))
        return ERROR_NO_MEMORY;

    *packet = &queue->packets[queue->tail];

    queue->tail = (queue->tail + 1u) % PACKET_QUEUE_SIZE;

    return SUCCESS;
}

uint32_t packet_queue_add(packet_queue_t * queue, radio_packet_t * packet)
{
    uint32_t err_code;
    radio_packet_t * new_packet;

    err_code = packet_queue_new(queue, &new_packet);
    if (err_code != SUCCESS)
        return err_code;

    memcpy(new_packet, packet, sizeof(*packet));

    return SUCCESS;
}

uint32_t packet_queue_get(packet_queue_t * queue, radio_packet_t ** packet)
{
    if (packet_queue_is_empty(queue))
        return ERROR_NOT_FOUND;

    *packet = &queue->packets[queue->head];

    queue->head = (queue->head + 1u) % PACKET_QUEUE_SIZE;

    return SUCCESS;
}

