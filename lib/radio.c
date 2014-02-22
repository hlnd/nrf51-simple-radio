#include "radio.h"

#include <stdint.h>
#include <string.h>

#include "error.h"
#include "evt_queue.h"
#include "packet_timer.h"
#include "tx_queue.h"
#include "rx_queue.h"

#include "nrf51.h"
#include "nrf51_bitfields.h"

#define PREPARE_TX(x) do \
    { \
        NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos | \
            RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos | \
            RADIO_SHORTS_DISABLED_TXEN_Enabled << RADIO_SHORTS_DISABLED_TXEN_Pos; \
    } while(0)

#define PREPARE_RX(x) do \
    { \
        NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos | \
            RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos | \
            RADIO_SHORTS_DISABLED_RXEN_Enabled << RADIO_SHORTS_DISABLED_RXEN_Pos; \
    } while(0)

#define PREPARE_DISABLE(x) do \
    { \
        NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos | \
            RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos; \
    } while(0)

#define SETUP_INTERRUPTS(x) do \
    { \
        NRF_RADIO->INTENSET = RADIO_INTENSET_END_Enabled << RADIO_INTENSET_END_Pos | \
            RADIO_INTENSET_DISABLED_Enabled << RADIO_INTENSET_DISABLED_Pos; \
    } while(0)

typedef enum
{
    IDLE,
    RX_PACKET_RECEIVE,
    RX_ACK_SEND,
    TX_PACKET_SEND,
    TX_ACK_RECEIVE,
} radio_state_t;

volatile static radio_state_t m_state;

static radio_packet_t m_tx_packet;
static radio_packet_t m_rx_packet;

static uint8_t m_tx_attempt_count;

static void tx_packet_prepare(void)
{
    uint32_t err_code;
    err_code = tx_queue_get(&m_tx_packet);
    ASSUME_SUCCESS(err_code);

    m_tx_attempt_count = 0;

    NRF_RADIO->PACKETPTR = (uint32_t) &m_tx_packet;
}

void RADIO_IRQHandler(void)
{
    radio_evt_type_t evt_type;
    uint32_t err_code;

    if((NRF_RADIO->EVENTS_END == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_END_Msk))
    {
        NRF_RADIO->EVENTS_END = 0;

        switch (m_state)
        {
            case TX_PACKET_SEND:
                NRF_RADIO->PACKETPTR = (uint32_t) &m_rx_packet;

                m_state = TX_ACK_RECEIVE;
                break;

            case TX_ACK_RECEIVE:
                packet_timer_evt_handler(PACKET_TIMER_EVT_PACKET_RX);

                if (m_rx_packet.flags.ack == 1)
                    evt_type = PACKET_SENT;
                else
                    evt_type = PACKET_LOST;

                evt_queue_add(evt_type);

                if (tx_queue_is_empty())
                {
                    m_state = IDLE;
                }
                else
                {
                    tx_packet_prepare();
                    m_state = TX_PACKET_SEND;
                }
                break;

            case RX_PACKET_RECEIVE:
                m_tx_packet.flags.ack = 0;

                err_code = rx_queue_add(&m_rx_packet);
                if (err_code == SUCCESS)
                {
                    m_tx_packet.flags.ack = 1;

                    evt_queue_add(PACKET_RECEIVED);
                }
                NRF_RADIO->PACKETPTR = (uint32_t) &m_tx_packet;

                m_state = RX_ACK_SEND;
                break;

            case RX_ACK_SEND:
                NRF_RADIO->PACKETPTR = (uint32_t) &m_rx_packet;

                m_state = RX_PACKET_RECEIVE;
                break;

            case IDLE:
                break;
        }
    }
    if ((NRF_RADIO->EVENTS_DISABLED == 1) && (NRF_RADIO->INTENSET & RADIO_INTENSET_DISABLED_Msk))
    {
        NRF_RADIO->EVENTS_DISABLED = 0; 

        switch (m_state)
        {
            case TX_PACKET_SEND:
            case RX_ACK_SEND:
                PREPARE_RX();
                break;

            case RX_PACKET_RECEIVE:
                PREPARE_TX();
                break;

            case TX_ACK_RECEIVE:
                if (tx_queue_is_empty())
                    PREPARE_DISABLE();
                else
                    PREPARE_TX();
                break;

            case IDLE:
                packet_timer_event_stop();
                break;
        }
    }
}

void on_packet_timer_timeout(void)
{
    if (m_tx_attempt_count++ < RADIO_TX_ATTEMPT_MAX)
    {
        PREPARE_TX();
        m_state = TX_PACKET_SEND;
    } 
    else
    {
        packet_timer_event_stop();
        evt_queue_add(PACKET_LOST);
    }
}

uint32_t radio_init(radio_evt_handler_t * evt_handler)
{
    m_state = IDLE;

    evt_queue_init(evt_handler);
    tx_queue_init();
    rx_queue_init();

    NRF_RADIO->BASE0 = 0xE7E7E7E7;
    NRF_RADIO->PREFIX0 = 0xE7E7E7E7;

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

    return SUCCESS;
}

uint32_t radio_send(radio_packet_t * packet)
{
    uint32_t err_code;

    err_code = tx_queue_add(packet);
    if (err_code != SUCCESS)
        return err_code;

    if (m_state != IDLE)
        return SUCCESS;

    m_state = TX_PACKET_SEND;

    packet_timer_tx_prepare(on_packet_timer_timeout);

    NRF_RADIO->RXADDRESSES = RADIO_RXADDRESSES_ADDR0_Enabled << RADIO_RXADDRESSES_ADDR0_Pos;

        SETUP_INTERRUPTS();
    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos |
        RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos |
        RADIO_SHORTS_DISABLED_RXEN_Enabled << RADIO_SHORTS_DISABLED_RXEN_Pos;

    NVIC_SetPriority(RADIO_IRQn, 0);
    NVIC_EnableIRQ(RADIO_IRQn);

    tx_packet_prepare();

    packet_timer_event_start();

    return SUCCESS;
}

uint32_t radio_receive_start(void)
{
    m_state = RX_PACKET_RECEIVE;

    packet_timer_rx_prepare();

    NRF_RADIO->RXADDRESSES = RADIO_RXADDRESSES_ADDR0_Enabled << RADIO_RXADDRESSES_ADDR0_Pos;

    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos |
        RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos |
        RADIO_SHORTS_DISABLED_TXEN_Enabled << RADIO_SHORTS_DISABLED_TXEN_Pos;

    SETUP_INTERRUPTS();
    NVIC_EnableIRQ(RADIO_IRQn);

    NRF_RADIO->PACKETPTR = (uint32_t) &m_rx_packet;

    return SUCCESS;
}
