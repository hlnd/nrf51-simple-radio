#include "tx_queue.h"

#include <stdint.h>

#include "queue.h"

static queue_t m_tx_queue;

void tx_queue_init(void)
{
    QUEUE_INIT(m_tx_queue, RADIO_PACKET_QUEUE_SIZE, sizeof(radio_packet_t));
}

uint32_t tx_queue_get(radio_packet_t * packet)
{
    return queue_get(&m_tx_queue, (uint8_t *) packet);
}

uint32_t tx_queue_add(radio_packet_t * packet)
{
    return queue_add(&m_tx_queue, (uint8_t *) packet);
}


uint32_t tx_queue_new(radio_packet_t ** packet)
{
    return queue_new(&m_tx_queue, (uint8_t **) packet);
}

uint32_t tx_queue_get_ptr(radio_packet_t ** packet)
{
    return queue_get_ptr(&m_tx_queue, (uint8_t **) packet);
}

bool tx_queue_is_empty(void)
{
    return queue_is_empty(&m_tx_queue);
}
