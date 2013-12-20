#include "rx_queue.h"

#include <stdint.h>

#include "queue.h"

static queue_t m_rx_queue;

void rx_queue_init(void)
{
    QUEUE_INIT(m_rx_queue, RADIO_PACKET_QUEUE_SIZE, sizeof(radio_packet_t));
}

uint32_t rx_queue_get(radio_packet_t * packet)
{
    return queue_get(&m_rx_queue, (uint8_t *) packet);
}

uint32_t rx_queue_add(radio_packet_t * packet)
{
    return queue_add(&m_rx_queue, (uint8_t *) packet);
}

bool rx_queue_is_empty(void)
{
    return queue_is_empty(&m_rx_queue);
}
