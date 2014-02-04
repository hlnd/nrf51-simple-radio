#include <stdint.h>

#include "nrf51.h"
#include "nrf_delay.h"

#include "buttons.h"
#include "leds.h"

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

void button_evt_handler(uint8_t pin_number, bool is_pressed)
{

}

int main(void)
{
    buttons_init(button_evt_handler);
    leds_init();

    while (1)
    {
        led_toggle(LED0);
        nrf_delay_us(100000);
    }
}


