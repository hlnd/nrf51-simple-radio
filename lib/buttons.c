#include "buttons.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "radio.h"
#include "evt_queue.h"
#include "nrf.h"

static uint32_t prev_button0_state = 1;
static uint32_t prev_button1_state = 1;
static uint32_t button0_state;
static uint32_t button1_state;

static volatile bool in_debounce[2] = {false, false};

static button_evt_handler_t m_evt_handler;

void buttons_init(button_evt_handler_t evt_handler)
{
    m_evt_handler = evt_handler;

    nrf_gpio_cfg_input(BUTTON0, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON1, NRF_GPIO_PIN_PULLUP);
    nrf_gpiote_event_config(0, BUTTON0, NRF_GPIOTE_POLARITY_TOGGLE);
    nrf_gpiote_event_config(1, BUTTON1, NRF_GPIOTE_POLARITY_TOGGLE);
        
    NRF_GPIOTE->INTENSET = (GPIOTE_INTENSET_IN0_Enabled << GPIOTE_INTENSET_IN0_Pos);
    NRF_GPIOTE->INTENSET = (GPIOTE_INTENSET_IN1_Enabled << GPIOTE_INTENSET_IN1_Pos);
    
    NVIC_SetPriority(GPIOTE_IRQn, 2);
    NVIC_EnableIRQ(GPIOTE_IRQn);
    
    NRF_TIMER1->PRESCALER = 4; // 1us ticks
    NRF_TIMER1->TASKS_START = 1;
    NRF_TIMER1->CC[1] = DEBOUNCE_TIME_US_BUTTON1;
    //NRF_TIMER1->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos;
    NRF_TIMER1->INTENSET =     TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos | 
                            TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos;
    
    NVIC_SetPriority(TIMER1_IRQn, 1);
    NVIC_EnableIRQ(TIMER1_IRQn);
}

void start_debounce(int button_index)
{
    if (!in_debounce[button_index])
    {
        NRF_TIMER1->TASKS_CAPTURE[2] = 1;
        NRF_TIMER1->CC[button_index] = (NRF_TIMER1->CC[2] + DEBOUNCE_TIME_US_BUTTON0);
        if (button_index == 0)
            NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
        else if (button_index == 1)
            NRF_TIMER1->INTENSET = TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos;
        in_debounce[button_index] = true;
    }
}

void stop_debounce(int button_index)
{
    if (in_debounce[button_index])
    {
        if (button_index == 0)
            NRF_TIMER1->INTENCLR = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
        else if (button_index == 1)
            NRF_TIMER1->INTENCLR = TIMER_INTENSET_COMPARE1_Enabled << TIMER_INTENSET_COMPARE1_Pos;
        in_debounce[button_index] = false;
    }
}

uint32_t button_state(button_t pin)
{
    return ((NRF_GPIO->IN >> (uint32_t)pin) & 1UL);
}

void GPIOTE_IRQHandler(void)
{
    
    if (NRF_GPIOTE->EVENTS_IN[0] == 1)
    {
        NRF_GPIOTE->EVENTS_IN[0] = 0;
        
        button0_state = button_state(BUTTON0);
        if (!in_debounce[0])
        {
            if (button0_state && !prev_button0_state) (*m_evt_handler)(BUTTON0, button0_state);
            if (!button0_state && prev_button0_state) (*m_evt_handler)(BUTTON0, button0_state);
            
            if (prev_button0_state != button0_state)
            {
                start_debounce(0);
                prev_button0_state = button0_state;
            }
        }
    }
        
    if (NRF_GPIOTE->EVENTS_IN[1] == 1)
    {
        NRF_GPIOTE->EVENTS_IN[1] = 0;
        
        button1_state = button_state(BUTTON1);
        if (!in_debounce[1])
        {
            if (button1_state && !prev_button1_state) (*m_evt_handler)(BUTTON1, button1_state);
            if (!button1_state && prev_button1_state) (*m_evt_handler)(BUTTON1, button1_state);
            
            if (prev_button1_state != button1_state)
            {
                start_debounce(1);
                prev_button1_state = button1_state;
            }
        }
    }
}

void TIMER1_IRQHandler(void)
{
    if (NRF_TIMER1->EVENTS_COMPARE[0] == 1)
    {
        NRF_TIMER1->EVENTS_COMPARE[0] = 0;
        stop_debounce(0);
    }
    else if (NRF_TIMER1->EVENTS_COMPARE[1] == 1)
    {
        NRF_TIMER1->EVENTS_COMPARE[1] = 0;
        stop_debounce(1);
    }
}
