#include "packet_timer.h"

#include "nrf51.h"
#include "nrf51_bitfields.h"

packet_timer_timeout_callback m_timeout_callback;


void TIMER0_IRQHandler(void)
{
    if (NRF_TIMER0->EVENTS_COMPARE[0] == 1 && NRF_TIMER0->INTENSET & (TIMER_INTENSET_COMPARE0_Msk))
    {
        NRF_TIMER0->EVENTS_COMPARE[0] = 0;
    }
    if (NRF_TIMER0->EVENTS_COMPARE[1] == 1 && NRF_TIMER0->INTENSET & (TIMER_INTENSET_COMPARE1_Msk))
    {
        NRF_TIMER0->EVENTS_COMPARE[1] = 0;

        (*m_timeout_callback)();
    }
}

static void hfclk_start(void)
{
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
}

static void hfclk_stop(void)
{
    NRF_CLOCK->TASKS_HFCLKSTOP = 1;
}


void packet_timer_evt_handler(packet_timer_evt_t evt)
{
    switch (evt)
    {
        case PACKET_TIMER_EVT_PACKET_RX:
            NRF_TIMER0->INTENCLR = TIMER_INTENCLR_COMPARE1_Enabled << TIMER_INTENCLR_COMPARE1_Pos;
            break;
    }
}

void packet_timer_rx_prepare(void)
{
    hfclk_start();

    NRF_TIMER0->CC[0] = 1500;
    NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos |
                         TIMER_SHORTS_COMPARE0_STOP_Enabled << TIMER_SHORTS_COMPARE0_STOP_Pos;

    NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;

    // CH21: TIMER0->EVENTS_COMPARE[0] -> RADIO->TASKS_RXEN
    NRF_PPI->CHENSET = PPI_CHENSET_CH21_Enabled << PPI_CHENSET_CH21_Pos;
}

void packet_timer_tx_prepare(packet_timer_timeout_callback timeout_callback)
{
    m_timeout_callback = timeout_callback;

    hfclk_start();

    NRF_TIMER0->PRESCALER = 4;

    NRF_TIMER0->CC[0] = 1500;
    NRF_TIMER0->CC[1] = 4000;
    NRF_TIMER0->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos;
    NRF_TIMER0->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos | 
                           TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos;
    NVIC_SetPriority(TIMER0_IRQn, 0);
    NVIC_EnableIRQ(TIMER0_IRQn);

    // CH20: TIMER0->EVENTS_COMPARE[0] -> RADIO->TASKS_TXEN
    // CH22: TIMER0->EVENTS_COMPARE[1] -> RADIO->TASKS_DISABLE
    NRF_PPI->CHENSET = PPI_CHENSET_CH20_Enabled << PPI_CHENSET_CH20_Pos |
                       PPI_CHENSET_CH22_Enabled << PPI_CHENSET_CH22_Pos;
}

void packet_timer_event_start(void)
{
    NRF_TIMER0->TASKS_START = 1;
}

void packet_timer_event_stop(void)
{
    hfclk_stop();
    
    NRF_TIMER0->TASKS_STOP = 1;
    NRF_TIMER0->TASKS_CLEAR = 1;
}
