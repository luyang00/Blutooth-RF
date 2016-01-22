#ifndef __TIME_H
#define	__TIME_H

#include "nrf51.h"

#define TIMER_DELAY_MS               (1000UL)   /**< Timer Delay in milli-seconds. */


/** @def p_timer
 * Enumeration for the timers available for NRF device.
 */
typedef enum
{
  TIMER0 = 0,  /**< Timer 0 module, base address at 0x40008000. */
  TIMER1,      /**< Timer 1 module, base address at 0x40009000. */
  TIMER2       /**< Timer 2 module, base address at 0x4000A000. */
} timer_t;

void nrf_timer_delay_ms(timer_t timer, uint_fast16_t volatile number_of_ms);



#endif /* __TIME_H */
