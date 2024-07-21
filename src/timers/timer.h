#ifndef __TIMER_H
#define __TIMER_H
#include <stdint.h>

#include "lpc17xx.h"
#include "../utils/timers_utils.h"


// Some useful defines for MCR values
// Remember that (regarding MR0): MCR[0]=Interrupt, MCR[1]=Reset, MCR[2]=Stop
#define MCR_INTERRUPT_AND_RESET_ON_MR0 3
#define MCR_INTERRUPT_RESET_AND_STOP_ON_MR0 7


typedef enum {
    TIMER0 = TIM0,
    TIMER1,
    TIMER2,
    TIMER3,
} Timer;


/* lib_timer.c */
extern void init_timer(Timer timer, uint32_t ms, uint8_t mcr);
extern void enable_timer(Timer timer);
extern void disable_timer(Timer timer);
extern void reset_timer(Timer timer);

/* IRQ_timer.c */
extern void TIMER0_IRQHandler(void);
extern void TIMER1_IRQHandler(void);



#endif /* end __TIMER_H */
