#include <stdio.h>

#include "nrf51.h"
#include "nrf_delay.h"

#include "radio.h"

void radio_evt_handler(radio_evt_t * evt)
{
    switch (evt->type)
    {
        case PACKET_RECEIVED:
            NRF_GPIO->OUTSET = 1 << 18;
            nrf_delay_us(100000);
            NRF_GPIO->OUTCLR = 1 << 18;
            break;
    }
}

int main(void)
{
    radio_init(radio_evt_handler);

    radio_receive_start();

    NRF_GPIO->DIR = (1 << 18) | (1 << 19);
    while (1)
    {
        NRF_GPIO->OUTSET = 1 << 19;
        nrf_delay_us(100000);
        NRF_GPIO->OUTCLR = 1 << 19;
        nrf_delay_us(100000);
    }
}
