#ifndef PACKET_H_INCLUDED
#define PACKET_H_INCLUDED
#include <stdint.h>
#include <stdbool.h>

#define RADIO_PACKET_MAX_LEN 64
#define RADIO_PACKET_QUEUE_SIZE 8

typedef struct __attribute__((packed))
{
    uint8_t len;
    struct __attribute__((packed))
    {
        uint8_t padding : 8;
        //uint8_t ack : 1;
    } flags;
    uint8_t data[RADIO_PACKET_MAX_LEN];
} radio_packet_t;

uint32_t dummy_packet_get(radio_packet_t ** packet);

bool packet_is_empty(radio_packet_t* packet);

#endif
