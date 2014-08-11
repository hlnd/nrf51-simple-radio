#include "radio.h"

#include <stdint.h>
#include <string.h>

#include "error.h"
#include "evt_queue.h"
#include "tx_queue.h"
#include "rx_queue.h"
#include "packet.h"

#include "nrf51.h"
#include "nrf51_bitfields.h"


typedef enum
{
    IDLE,
    RX_PACKET_RECEIVE,
    TX_PACKET_SEND,
} radio_state_t;

volatile radio_state_t m_state;

static radio_packet_t * m_tx_packet;
static radio_packet_t * m_rx_packet;
static radio_packet_t * m_initial_packet;

uint32_t prepare_rx(void);
uint32_t prepare_tx(void);
uint32_t prepare_idle(void);

uint32_t radio_init(radio_evt_handler_t * evt_handler, radio_packet_t * initial_packet)
{
    m_state = IDLE;

    m_initial_packet = initial_packet;

    evt_queue_init(evt_handler);
    tx_queue_init();
    rx_queue_init();

    NRF_RADIO->BASE0 = 0xE7E7E7E7;
    NRF_RADIO->PREFIX0 = 0xE7E7E7E7;

    NRF_RADIO->RXADDRESSES = RADIO_RXADDRESSES_ADDR0_Enabled << RADIO_RXADDRESSES_ADDR0_Pos;

    NRF_RADIO->PCNF0 = 8 << RADIO_PCNF0_LFLEN_Pos |
        8 << RADIO_PCNF0_S1LEN_Pos;
    NRF_RADIO->PCNF1 = 64 << RADIO_PCNF1_MAXLEN_Pos |
        4 << RADIO_PCNF1_BALEN_Pos;

    NRF_RADIO->FREQUENCY = 40;
    NRF_RADIO->TIFS = 150;
    NRF_RADIO->MODE = RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos;

    NRF_RADIO->CRCCNF = RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos;

    NRF_RADIO->CRCINIT = 0xFFFF;
    NRF_RADIO->CRCPOLY = 0x11021;

    NRF_RADIO->INTENSET = RADIO_INTENSET_END_Enabled << RADIO_INTENSET_END_Pos;
    NVIC_SetPriority(RADIO_IRQn, 0);
    NVIC_EnableIRQ(RADIO_IRQn);

    return SUCCESS;
}

uint32_t radio_start_tx()
{
    uint32_t err_code;

    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    NRF_RADIO->TASKS_TXEN = 1;

    err_code = prepare_tx();

    return err_code;
}

uint32_t radio_start_rx()
{
    uint32_t err_code = SUCCESS;

    if (m_state == IDLE)
    {
        NRF_CLOCK->TASKS_HFCLKSTART = 1;

        NRF_RADIO->TASKS_RXEN = 1;

        err_code = prepare_rx();
    }

    return err_code;
}

uint32_t prepare_tx()
{
    uint32_t err_code = SUCCESS;
    err_code = tx_queue_get_ptr(&m_tx_packet);

    if (err_code == ERROR_NOT_FOUND)
    {
        if (m_state == IDLE)
        {
            m_tx_packet = m_initial_packet;
            err_code = SUCCESS;
        }
        else
        {
            err_code = dummy_packet_get(&m_tx_packet);
        }
    }
    NRF_RADIO->PACKETPTR = (uint32_t)m_tx_packet;
    m_state = TX_PACKET_SEND;

    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos |
                        RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos |
                        RADIO_SHORTS_DISABLED_RXEN_Enabled << RADIO_SHORTS_DISABLED_RXEN_Pos;

    return err_code;
}

uint32_t prepare_rx()
{
    uint32_t err_code;
    err_code = rx_queue_new(&m_rx_packet);

    if (err_code == SUCCESS)
    {
        NRF_RADIO->PACKETPTR = (uint32_t)m_rx_packet;
        m_state = RX_PACKET_RECEIVE;

        NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos |
                            RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos |
                            RADIO_SHORTS_DISABLED_TXEN_Enabled << RADIO_SHORTS_DISABLED_TXEN_Pos;
    }

    return err_code;
}

uint32_t prepare_idle()
{
    m_state = IDLE;

    NRF_RADIO->SHORTS = 0;

    NRF_RADIO->TASKS_DISABLE = 1;

    return SUCCESS;
}

void RADIO_IRQHandler(void)
{
    uint32_t err_code = SUCCESS;
    uint32_t temp = NRF_RADIO->STATE;

    if((NRF_RADIO->EVENTS_END == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_END_Msk))
    {
        NRF_RADIO->EVENTS_END = 0;
        switch(m_state)
        {
            case RX_PACKET_RECEIVE:
                evt_queue_add(PACKET_RECEIVED);
                rx_queue_ready();

                if (tx_queue_is_empty() && packet_is_empty(m_rx_packet))
                    err_code = prepare_idle();
                else
                    err_code = prepare_tx();
                break;

            case TX_PACKET_SEND:
                evt_queue_add(PACKET_SENT);

                err_code = prepare_rx();
                break;

            default:
                break;
        }
    }

    ASSUME_SUCCESS(err_code);
}

uint32_t radio_stop_rx(void)
{
    prepare_idle();
    return SUCCESS;
}

uint32_t radio_send(radio_packet_t * packet)
{
    uint32_t err_code = tx_queue_add(packet);
    if (m_state == IDLE)
        radio_start_tx();
    return err_code;
}
