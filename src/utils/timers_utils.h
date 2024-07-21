#ifndef __TIMERS_UTILS_H 
#define __TIMERS_UTILS_H 

#include "LPC17xx.h"

// Get the system frequency (defined in system_LPC17xx.h)
extern uint32_t SystemFrequency;


typedef enum {
    TIM0,
    TIM1,
    TIM2,
    TIM3,
    RIT
} TimerType;


/**
 * Given the wanted timer, automatically calculates the match 
 * register value based on the correct peripheral clock.
 * 
 * @code
 * // Sets the MR0 for TIMER0 to 50ms
 * LPC_TIM0->MR0 = get_match_for_timer(TIMER0, 50); 
 * @endcode
 * 
 * @param timer The timer you want
 * @param ms The time in milliseconds
*/
uint32_t get_match_for_timer(TimerType timer, uint32_t ms);


#endif /* __TIMERS_UTILS_H */
