#include "radio.h"

#include <stdint.h>
#include <string.h>

#include "error.h"
#include "nrf51.h"
#include "nrf51_bitfields.h"

static radio_evt_handler_t * m_evt_handler;

radio_packet_t m_rx_buffer[RADIO_PACKET_BUFFER_SIZE];
radio_packet_t m_tx_buffer[RADIO_PACKET_BUFFER_SIZE];

void RADIO_IRQHandler(void)
{
    if(NRF_RADIO->EVENTS_END)
    {
        NRF_RADIO->EVENTS_END = 0;

        radio_packet_t * packet = (radio_packet_t *) NRF_RADIO->PACKETPTR;
        memcpy(&m_rx_buffer[0], packet, sizeof(packet));

        radio_evt_t evt;
        evt.type = PACKET_RECEIVED;
        evt.packet = m_rx_buffer[0];

        (*m_evt_handler)(&evt);
    }
}

static void hfclk_start(void)
{
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while(!NRF_CLOCK->EVENTS_HFCLKSTARTED)
    {
    }
}

uint32_t radio_init(radio_evt_handler_t * evt_handler)
{
    m_evt_handler = evt_handler;

    NRF_RADIO->BASE0 = 0xE7E7E7E7;
    NRF_RADIO->PREFIX0 = 0xE7E7E7E7;

    NRF_RADIO->PCNF0 = 8 << RADIO_PCNF0_LFLEN_Pos;
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
    memcpy(&m_tx_buffer[0], packet, sizeof(packet));

    hfclk_start();

    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos |
        RADIO_SHORTS_END_DISABLE_Enabled << RADIO_SHORTS_END_DISABLE_Pos;

    NRF_RADIO->PACKETPTR = (uint32_t) &m_tx_buffer[0];
    NRF_RADIO->TASKS_TXEN = 1;

    return SUCCESS;
}

uint32_t radio_receive_start(void)
{
    hfclk_start();

    NRF_RADIO->RXADDRESSES = RADIO_RXADDRESSES_ADDR0_Enabled << RADIO_RXADDRESSES_ADDR0_Pos;

    NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Enabled << RADIO_SHORTS_READY_START_Pos |
        RADIO_SHORTS_END_START_Enabled << RADIO_SHORTS_END_START_Pos;

    NRF_RADIO->PACKETPTR = (uint32_t) &m_rx_buffer[0];

    NRF_RADIO->INTENSET = RADIO_INTENSET_END_Enabled << RADIO_INTENSET_END_Pos;
    NVIC_EnableIRQ(RADIO_IRQn);

    NRF_RADIO->TASKS_RXEN = 1;

    return SUCCESS;
}
