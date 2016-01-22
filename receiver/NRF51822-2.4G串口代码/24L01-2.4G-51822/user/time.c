#include "nrf51.h"
#include "nrf_gpio.h"
#include "led.h"
#include "time.h"
#include <stdbool.h>
#include <stdint.h>



/**
 * @brief Function for timer initialization.
 */
static volatile NRF_TIMER_Type * timer_init(timer_t timer)
{
    volatile NRF_TIMER_Type * p_timer;

    // 开始16 MHz晶振.
    NRF_CLOCK->EVENTS_HFCLKSTARTED  = 0;
    NRF_CLOCK->TASKS_HFCLKSTART     = 1;

    // 等待外部振荡器启动
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) 
    {
        // Do nothing.
    }

    switch (timer)
    {
        case TIMER0:
            p_timer = NRF_TIMER0;
            break;

        case TIMER1:
            p_timer = NRF_TIMER1;
            break;

        case TIMER2:
            p_timer = NRF_TIMER2;
            break;

        default:
            p_timer = 0;
            break;
    }
    return p_timer;
}


/** @brief Function for using the peripheral hardware timers to generate an event after requested number of milliseconds.
 *
 * @param[in] timer Timer to be used for delay, values from @ref p_timer
 * @param[in] number_of_ms Number of milliseconds the timer will count.
 * @note This function will power ON the requested timer, wait until the delay, and then power OFF that timer.
 */

void nrf_timer_delay_ms(timer_t timer, uint_fast16_t volatile number_of_ms)
{
    volatile NRF_TIMER_Type * p_timer = timer_init(timer);

    if (p_timer == 0) 
    {
        while(true) 
        {
            // Do nothing.
        }
    }

    p_timer->MODE           = TIMER_MODE_MODE_Timer;        // 设置为定时器模式
    p_timer->PRESCALER      = 9;                            // Prescaler 9 produces 31250 Hz timer frequency => 1 tick = 32 us.
    p_timer->BITMODE        = TIMER_BITMODE_BITMODE_16Bit;  // 16 bit 模式.
    p_timer->TASKS_CLEAR    = 1;                            // 清定时器.
    
    // With 32 us ticks, we need to multiply by 31.25 to get milliseconds.
    p_timer->CC[0]          = number_of_ms * 31;
    p_timer->CC[0]         += number_of_ms / 4; 
    p_timer->TASKS_START    = 1;                    // Start timer.

    while (p_timer->EVENTS_COMPARE[0] == 0)
    {
        // Do nothing.
    }

    p_timer->EVENTS_COMPARE[0]  = 0;
    p_timer->TASKS_STOP         = 1;                // Stop timer.
}
/** @} */

