#include "packet_queue.h"

#include "error.h"

#include <stdint.h>


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
    return abs(queue->head - queue->tail) == PACKET_QUEUE_SIZE;
}

uint32_t packet_queue_add(packet_queue_t * queue, radio_packet_t * packet)
{

}

uint32_t packet_queue_get(packet_queue_t * queue, radio_packet_t * packet)
{

}

