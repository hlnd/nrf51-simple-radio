#ifndef LEDS_H_INCLUDED
#define LEDS_H_INCLUDED

#include "gpio.h"

#if defined(BOARD_PCA10001)
#define LED_START 18
#define LED0 18
#define LED1 19
#define LED_STOP 20

#elif defined(BOARD_PCA10000)
#define LED_START 21
#define LED0 21
#define LED1 22
#define LED2 23
#define LED_STOP 24

#define LEDS_ACTIVE_LOW 1
#else 
#error "Board not defined."

#endif

static inline void leds_init(void)
{
    gpio_pins_cfg_out(LED_START, LED_STOP);
#ifdef LEDS_ACTIVE_LOW
    gpio_pins_set(LED_START, LED_STOP);
#endif
}

static inline void led_toggle(uint32_t pin)
{
    gpio_pin_toggle(pin);
}

static inline void led_on(uint32_t pin)
{
#ifdef LEDS_ACTIVE_LOW
    gpio_pin_clear(pin);
#else
    gpio_pin_set(pin);
#endif
}

static inline void leds_on(uint32_t pin_start, uint32_t pin_stop)
{
#ifdef LEDS_ACTIVE_LOW
    gpio_pins_clear(pin_start, pin_stop);
#else
    gpio_pins_set(pin_start, pin_stop);
#endif
}

static inline void led_off(uint32_t pin)
{
#ifdef LEDS_ACTIVE_LOW
    gpio_pin_set(pin);
#else
    gpio_pin_clear(pin);
#endif
}

static inline void leds_off(uint32_t pin_start, uint32_t pin_stop)
{
#ifdef LEDS_ACTIVE_LOW
    gpio_pins_set(pin_start, pin_stop);
#else
    gpio_pins_clear(pin_start, pin_stop);
#endif
}

#endif
