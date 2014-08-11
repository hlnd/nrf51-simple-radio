#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include <stdint.h>
#include "packet.h"

#ifdef __CC_ARM
#pragma anon_unions
#endif

#define RADIO_EVT_QUEUE_SIZE 12
#define RADIO_TX_ATTEMPT_MAX 3

typedef enum
{
    PACKET_RECEIVED,
    PACKET_SENT,
    PACKET_LOST,
} radio_evt_type_t;


typedef struct
{
   radio_evt_type_t type;
   union
   {
       radio_packet_t packet;
   };
} radio_evt_t;

typedef void (radio_evt_handler_t)(radio_evt_t * evt);

uint32_t radio_init(radio_evt_handler_t * evt_handler, radio_packet_t * initial_packet);
uint32_t radio_start_tx(void);
uint32_t radio_start_rx(void);
uint32_t radio_stop_rx(void);
uint32_t radio_send(radio_packet_t * packet);

#endif
