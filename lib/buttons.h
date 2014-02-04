#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include "gpio.h"
#include "stdint.h"

#define BUTTON0_PIN 16
#define BUTTON1_PIN 17

#define DEBOUNCE_TIME_US_BUTTON0 (5000)
#define DEBOUNCE_TIME_US_BUTTON1 (5000)

typedef void (*button_evt_handler_t)(uint8_t pin_no, bool is_pressed);

//typedef enum
//{
//    BUTTON0_ON,
//    BUTTON0_OFF,
//    BUTTON1_ON,
//    BUTTON1_OFF,
//} button_evt_type_t;

typedef enum
{
    BUTTON0 = BUTTON0_PIN,
    BUTTON1 = BUTTON1_PIN,
} button_t;

void buttons_init(button_evt_handler_t evt_handler);



#endif
