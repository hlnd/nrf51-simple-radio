#include "radio.h"

#include <stdint.h>
#include <string.h>

#include "error.h"
#include "queue.h"

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

typedef enum
{
    IDLE,
    RX_PACKET_RECEIVE,
    RX_ACK_SEND,
    TX_PACKET_SEND,
    TX_ACK_RECEIVE,
} radio_state_t;

volatile static radio_state_t m_state;

static radio_evt_handler_t * m_evt_handler;

static queue_t m_tx_queue;
static queue_t m_rx_queue;
static queue_t m_evt_queue;

static radio_packet_t m_tx_packet;
static radio_packet_t m_rx_packet;

static uint8_t m_tx_attempt_count;

static void hfclk_start(void)
{
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_HFCLKSTARTED != 1)
    {
    }
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
}

static void hfclk_stop(void)
{
    NRF_CLOCK->TASKS_HFCLKSTOP = 1;
}

static void tx_packet_prepare(void)
{
    uint32_t err_code;
    err_code = queue_get(&m_tx_queue, (uint8_t *) &m_tx_packet);
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
                NRF_TIMER0->TASKS_STOP = 1;
                NRF_TIMER0->TASKS_CLEAR = 1;
                NRF_TIMER0->INTENCLR = TIMER_INTENCLR_COMPARE1_Enabled << TIMER_INTENCLR_COMPARE1_Pos;

                if (m_rx_packet.flags.ack == 1)
                    evt_type = PACKET_SENT;
                else
                    evt_type = PACKET_LOST;

                err_code = queue_add(&m_evt_queue, (uint8_t *) &evt_type);
                ASSUME_SUCCESS(err_code);

                NVIC_SetPendingIRQ(SWI0_IRQn);

                if (queue_is_empty(&m_tx_queue))
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

                err_code = queue_add(&m_rx_queue, (uint8_t *) &m_rx_packet);
                if (err_code == SUCCESS)
                {
                    m_tx_packet.flags.ack = 1;

                    evt_type = PACKET_RECEIVED;
                    err_code = queue_add(&m_evt_queue, (uint8_t *) &evt_type);
                    ASSUME_SUCCESS(err_code);

                    NVIC_SetPendingIRQ(SWI0_IRQn);
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
                if (queue_is_empty(&m_tx_queue))
                    PREPARE_DISABLE();
                else
                    PREPARE_TX();
                break;

            case IDLE:
                hfclk_stop();
                break;
        }
    }
}

void TIMER0_IRQHandler(void)
{
    uint32_t err_code;

    if (NRF_TIMER0->EVENTS_COMPARE[1] == 1 && NRF_TIMER0->INTENSET & (TIMER_INTENSET_COMPARE1_Msk))
    {
        NRF_TIMER0->EVENTS_COMPARE[1] = 0;

        if (m_tx_attempt_count++ < RADIO_TX_ATTEMPT_MAX)
        {
            PREPARE_TX();
            m_state = TX_PACKET_SEND;
        } 
        else
        {
            radio_evt_t evt;
            evt.type = PACKET_LOST;

            err_code = queue_add(&m_evt_queue, &evt);
            ASSUME_SUCCESS(err_code);

            NVIC_SetPendingIRQ(SWI0_IRQn);
        }
    }
}
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
                err_code = queue_get(&m_rx_queue, (uint8_t *) &evt.packet);
                ASSUME_SUCCESS(err_code);
                break;
        }

        (*m_evt_handler)(&evt);
    }
}

uint32_t radio_init(radio_evt_handler_t * evt_handler)
{
    m_evt_handler = evt_handler;
    m_state = IDLE;

    QUEUE_INIT(m_rx_queue, RADIO_PACKET_QUEUE_SIZE, sizeof(radio_packet_t));
    QUEUE_INIT(m_tx_queue, RADIO_PACKET_QUEUE_SIZE, sizeof(radio_packet_t));

    QUEUE_INIT(m_evt_queue, RADIO_EVT_QUEUE_SIZE, sizeof(radio_evt_type_t));

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

    NVIC_SetPriority(SWI0_IRQn, 2);
    NVIC_EnableIRQ(SWI0_IRQn);

    return SUCCESS;
}

uint32_t radio_send(radio_packet_t * packet)
{
    uint32_t err_code;

    err_code = queue_add(&m_tx_queue, (uint8_t *) packet);
    if (err_code != SUCCESS)
        return err_code;

    if (m_state != IDLE)
        return SUCCESS;

    m_state = TX_PACKET_SEND;

    hfclk_start();

    NRF_RADIO->RXADDRESSES = RADIO_RXADDRESSES_ADDR0_Enabled << RADIO_RXADDRESSES_ADDR0_Pos;

    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos |
        RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos |
        RADIO_SHORTS_DISABLED_RXEN_Enabled << RADIO_SHORTS_DISABLED_RXEN_Pos;

    NRF_RADIO->INTENSET = RADIO_INTENSET_END_Enabled << RADIO_INTENSET_END_Pos | 
        RADIO_INTENSET_DISABLED_Enabled << RADIO_INTENSET_DISABLED_Pos;
    NVIC_SetPriority(RADIO_IRQn, 0);
    NVIC_EnableIRQ(RADIO_IRQn);

    NRF_TIMER0->CC[1] = 1000;
    NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos;
    NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos;
    NVIC_SetPriority(TIMER0_IRQn, 0);
    NVIC_EnableIRQ(TIMER0_IRQn);

    NRF_PPI->CHENSET = PPI_CHENSET_CH22_Enabled << PPI_CHENSET_CH22_Pos;

    tx_packet_prepare();

    NRF_RADIO->TASKS_TXEN = 1;
    NRF_TIMER0->TASKS_START = 1;

    return SUCCESS;
}

uint32_t radio_receive_start(void)
{
    m_state = RX_PACKET_RECEIVE;

    hfclk_start();

    NRF_RADIO->RXADDRESSES = RADIO_RXADDRESSES_ADDR0_Enabled << RADIO_RXADDRESSES_ADDR0_Pos;

    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos |
        RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos |
        RADIO_SHORTS_DISABLED_TXEN_Enabled << RADIO_SHORTS_DISABLED_TXEN_Pos;

    NRF_RADIO->INTENSET = RADIO_INTENSET_END_Enabled << RADIO_INTENSET_END_Pos | 
        RADIO_INTENSET_DISABLED_Enabled << RADIO_INTENSET_DISABLED_Pos;
    NVIC_EnableIRQ(RADIO_IRQn);

    NRF_RADIO->PACKETPTR = (uint32_t) &m_rx_packet;
    NRF_RADIO->TASKS_RXEN = 1;

    return SUCCESS;
}
