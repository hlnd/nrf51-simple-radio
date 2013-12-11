#include <stdio.h>

#include "nrf51.h"
#include "nrf_delay.h"

#include "error.h"
#include "gpio.h"
#include "leds.h"
#include "radio.h"

void error_handler(uint32_t err_code, uint32_t line_num, char * file_name)
{
    while (1)
    {
        for (uint8_t i = LED_START; i < LED_STOP; i++)
        {
            led_toggle(i);
            nrf_delay_us(50000);
        }
    }
}

void radio_evt_handler(radio_evt_t * evt)
{
    switch (evt->type)
    {
        case PACKET_RECEIVED:
            led_toggle(LED1);
            break;

        default:
            break;
    }
}

int main(void)
{
    radio_init(radio_evt_handler);

    radio_receive_start();

    leds_init();
    gpio_pins_cfg_out(0, 8);

    while (1)
    {
        led_toggle(LED0);
        nrf_delay_us(100000);
    }
}
