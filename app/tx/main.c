#include <stdio.h>
#include <stdint.h>
#include <string.h>

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
        case PACKET_SENT:
            led_toggle(LED1);
            break;

        case PACKET_LOST:
            led_toggle(LED0);
            break;
    }

}

int main(void)
{
    uint8_t i = 0; 
    uint32_t err_code;

    leds_init();

    radio_packet_t packet;
    packet.len = 4;
    packet.flags.ack = 0;

    radio_init(radio_evt_handler);

    while (1)
    {
        packet.data[0] = i++;
        packet.data[1] = 0x12;
        err_code = radio_send(&packet);
        ASSUME_SUCCESS(err_code);

        packet.data[0] = i++;
        packet.data[1] = 0x12;
        err_code = radio_send(&packet);
        ASSUME_SUCCESS(err_code);

        nrf_delay_us(1000000);
    }
}
