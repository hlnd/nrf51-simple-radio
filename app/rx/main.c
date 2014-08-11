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
    static uint32_t err_code, temp;
    err_code = radio_init(radio_evt_handler, 0);
    ASSUME_SUCCESS(err_code);

    leds_init();

    while (1)
    {
        err_code = radio_start_rx();
        led_on(LED0);
        nrf_delay_us(50000);
        led_off(LED0);
        nrf_delay_us(100000);
    }
}
