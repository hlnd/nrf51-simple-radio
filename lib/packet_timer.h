#ifndef PACKET_TIMER_H_INCLUDED__
#define PACKET_TIMER_H_INCLUDED__

typedef void (*packet_timer_timeout_callback)(void);

typedef enum
{
    PACKET_TIMER_EVT_PACKET_RX,
} packet_timer_evt_t;


void packet_timer_evt_handler(packet_timer_evt_t * evt);

void packet_timer_tx_prepare(packet_timer_timeout_callback timeout_callback);

void packet_timer_rx_prepare(void);

void packet_timer_event_start(void);

void packet_timer_event_stop(void);

#endif
