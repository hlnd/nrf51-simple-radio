#include "evt_queue.h"

#include "nrf51.h"

#include "error.h"
#include "queue.h"
#include "rx_queue.h"


static radio_evt_handler_t * m_evt_handler;
static queue_t m_evt_queue;

void SWI0_IRQHandler(void)
{
    uint32_t err_code;

    while (!queue_is_empty(&m_evt_queue))
    {
        radio_evt_t evt;

        err_code = queue_get(&m_evt_queue, &evt.type);
        ASSUME_SUCCESS(err_code);

        switch (evt.type)
        {
            case PACKET_RECEIVED:
                err_code = rx_queue_get(&evt.packet);
                ASSUME_SUCCESS(err_code);
                break;

            default:
                break;
        }

        (*m_evt_handler)(&evt);
    }
}

uint32_t evt_queue_init(radio_evt_handler_t evt_handler)
{
    m_evt_handler = evt_handler;

    QUEUE_INIT(m_evt_queue, RADIO_EVT_QUEUE_SIZE, sizeof(radio_evt_type_t));

    NVIC_SetPriority(SWI0_IRQn, 2);
    NVIC_EnableIRQ(SWI0_IRQn);

    return SUCCESS;
}

void evt_queue_add(radio_evt_type_t evt_type)
{
    uint32_t err_code;

    err_code = queue_add(&m_evt_queue, (uint8_t *) &evt_type);
    ASSUME_SUCCESS(err_code);

    NVIC_SetPendingIRQ(SWI0_IRQn);
}
