#include <stdio.h>

#include "nrf51.h"
#include "nrf_delay.h"

#include "gpio.h"
#include "led.h"
#include "radio.h"

void radio_evt_handler(radio_evt_t * evt)
{
    switch (evt->type)
    {
        case PACKET_RECEIVED:
            gpio_pin_toggle(LED1);
            break;
    }
}

int main(void)
{
    radio_init(radio_evt_handler);

    radio_receive_start();

    gpio_pins_cfg_out(LED_START, LED_STOP);
    gpio_pins_cfg_out(0, 8);

    while (1)
    {
        gpio_pin_toggle(LED0);
        nrf_delay_us(100000);
    }
}
