#ifndef GPIO_H_INCLUDED
#define GPIO_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

#include "nrf51.h"
#include "nrf51_bitfields.h"

typedef enum
{
    GPIO_PULL_DISABLED = GPIO_PIN_CNF_PULL_Disabled,
    GPIO_PULL_DOWN = GPIO_PIN_CNF_PULL_Pulldown,
    GPIO_PULL_UP = GPIO_PIN_CNF_PULL_Pullup,
} gpio_pull_t;

typedef enum
{
    GPIO_SENSE_DISABLED = GPIO_PIN_CNF_SENSE_Disabled,
    GPIO_SENSE_HIGH = GPIO_PIN_CNF_SENSE_High,
    GPIO_SENSE_LOW = GPIO_PIN_CNF_SENSE_Low,
} gpio_sense_t;

static inline void gpio_pin_cfg_out(uint32_t pin)
{
    NRF_GPIO->DIRSET = 1 << pin;
}

static inline void gpio_pins_cfg_out(uint32_t pin_start, uint32_t pin_end)
{
    for (uint32_t pin = pin_start; pin < pin_end; pin++)
    {
        gpio_pin_cfg_out(pin);
    }
}

static inline void gpio_pin_cfg_in(uint32_t pin, gpio_pull_t pull, gpio_sense_t sense)
{
    NRF_GPIO->PIN_CNF[pin] = GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos |
        pull << GPIO_PIN_CNF_PULL_Pos |
        sense << GPIO_PIN_CNF_SENSE_Pos;
}

static inline void gpio_pins_cfg_in(uint32_t pin_start, uint32_t pin_end, gpio_pull_t pull, gpio_sense_t sense)
{
    for (uint32_t pin = pin_start; pin < pin_end; pin++)
    {
        gpio_pin_cfg_in(pin, pull, sense);
    }
}

static inline bool gpio_pin_is_set(uint32_t pin)
{
    return NRF_GPIO->OUT & (1 << pin);
}

static inline void gpio_pin_set(uint32_t pin)
{
    NRF_GPIO->OUTSET = 1 << pin;
}

static inline void gpio_pins_set(uint32_t pin_start, uint32_t pin_end)
{
    for (uint32_t pin = pin_start; pin < pin_end; pin++)
    {
        gpio_pin_set(pin);
    }
}

static inline void gpio_pin_clear(uint32_t pin)
{
    NRF_GPIO->OUTCLR = 1 << pin;
}

static inline void gpio_pins_clear(uint32_t pin_start, uint32_t pin_end)
{
    for (uint32_t pin = pin_start; pin < pin_end; pin++)
    {
        gpio_pin_clear(pin);
    }
}

static inline void gpio_pin_toggle(uint32_t pin)
{
    if (gpio_pin_is_set(pin))
        gpio_pin_clear(pin);
    else
        gpio_pin_set(pin);
}

#endif
