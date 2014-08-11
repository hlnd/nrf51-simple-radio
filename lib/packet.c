#include "packet.h"
#include "error.h"


uint32_t dummy_packet_get(radio_packet_t ** packet)
{
    static radio_packet_t empty_packet = 
    {
        .len = 0,
        .flags = 0x80
    };

    *packet = &empty_packet;
    return SUCCESS;
}

bool packet_is_empty(radio_packet_t* packet)
{
    return (packet->len == 0);
}
