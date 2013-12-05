#include <stdio.h>

#include "nrf51.h"
#include "nrf_delay.h"

#include "gpio.h"
#include "radio.h"

void radio_evt_handler(radio_evt_t * evt)
{
    switch (evt->type)
    {
        case PACKET_RECEIVED:
            gpio_pin_toggle(19);
            break;
    }
}

int main(void)
{
    radio_init(radio_evt_handler);

    radio_receive_start();

    gpio_pins_cfg_out(18, 20);
    gpio_pins_cfg_out(0, 8);

    while (1)
    {
        gpio_pin_toggle(18);
        nrf_delay_us(100000);
    }
}
