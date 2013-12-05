#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "nrf51.h"
#include "nrf_delay.h"

#include "error.h"
#include "gpio.h"
#include "radio.h"

void radio_evt_handler(radio_evt_t * evt)
{

}

int main(void)
{
    uint8_t i = 0; 

    radio_packet_t packet;
    packet.len = 4;
    packet.flags.ack = 0;

    radio_init(radio_evt_handler);

    gpio_pins_cfg_out(21, 24);
    gpio_pin_set(21);
    gpio_pin_set(22);
    gpio_pin_set(23);


    while (1)
    {
        packet.data[0] = i++;
        packet.data[1] = 0x12;
        radio_send(&packet);

        gpio_pin_toggle(21);
        nrf_delay_us(1000000);
    }
}
