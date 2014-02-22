#include <stdio.h>

#include "nrf51.h"
#include "nrf_delay.h"

#include "error.h"
#include "gpio.h"
#include "leds.h"
#include "radio.h"

volatile uint32_t n_packets_received = 0;

void error_handler(uint32_t err_code, uint32_t line_num, char * file_name)
{
    volatile uint32_t m_err_code = err_code;
    volatile uint32_t m_line_num = line_num;
    char * volatile m_file_name  = file_name;
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
            n_packets_received++;
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
        led_on(LED0);
        nrf_delay_us(1000);
        led_off(LED0);
        nrf_delay_us(100000);
    }
}
