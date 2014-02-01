#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include <stdint.h>

#pragma anon_unions
#define RADIO_PACKET_MAX_LEN 64
#define RADIO_PACKET_QUEUE_SIZE 8
#define RADIO_EVT_QUEUE_SIZE 8
#define RADIO_TX_ATTEMPT_MAX 3

typedef enum
{
    NA,
    PACKET_RECEIVED,
    PACKET_SENT,
    PACKET_LOST,
    BUTTON0_ON,
    BUTTON0_OFF,
    BUTTON1_ON,
    BUTTON1_OFF,
} radio_evt_type_t;

typedef struct __attribute__((packed))
{
    uint8_t len;
    struct __attribute__((packed))
    {
        uint8_t padding : 7;
        uint8_t ack : 1;
    } flags;
    uint8_t data[RADIO_PACKET_MAX_LEN];
} radio_packet_t;

typedef struct
{
   radio_evt_type_t type;
   union
   {
       radio_packet_t packet;
   };
} radio_evt_t;

typedef void (radio_evt_handler_t)(radio_evt_t * evt);

uint32_t radio_init(radio_evt_handler_t * evt_handler);
uint32_t radio_send(radio_packet_t * packet);
uint32_t radio_receive_start(void);

#endif
