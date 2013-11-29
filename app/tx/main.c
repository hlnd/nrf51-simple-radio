#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "nrf51.h"
#include "nrf_delay.h"

#include "error.h"
#include "radio.h"

void radio_event_handler(radio_evt_t * evt)
{

}

int main(void)
{
    uint8_t i = 0; 

    radio_packet_t packet;
    packet.len = 4;

    radio_init(radio_event_handler);

    NRF_GPIO->DIR = 1 << 18;

    while (1)
    {
        packet.data[0] = i++;
        packet.data[1] = 0x12;
        radio_send(&packet);

        NRF_GPIO->OUTSET = 1 << 18;
        nrf_delay_us(100000);
        NRF_GPIO->OUTCLR = 1 << 18;
        nrf_delay_us(100000);
    }
}
