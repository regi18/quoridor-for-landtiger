#include "timer.h"


/**
 * Init a timer to the given ms interval
 * 
 * @param timer The wanted timer
 * @param ms The period in milliseconds
 * @param mcr Sets the MCR value
*/
void init_timer(Timer timer, uint32_t ms, uint8_t mcr)
{
  uint32_t timer_interval = get_match_for_timer((TimerType)timer, ms);

  if (timer == TIMER0)
  {
    LPC_TIM0->MR0 = timer_interval;
    LPC_TIM0->MCR = mcr;
    NVIC_EnableIRQ(TIMER0_IRQn);
  }
  else if (timer == TIMER1)
  {
    LPC_TIM1->MR0 = timer_interval;
    LPC_TIM1->MCR = mcr;
    NVIC_EnableIRQ(TIMER1_IRQn);
  }
  else if (timer == TIMER2) {
    LPC_TIM2->MR0 = timer_interval;
    LPC_TIM2->MCR = mcr;
    NVIC_EnableIRQ(TIMER2_IRQn);
  }
  else if (timer == TIMER3) {
    LPC_TIM3->MR0 = timer_interval;
    LPC_TIM3->MCR = mcr;
    NVIC_EnableIRQ(TIMER3_IRQn);
  }
}


/**
 * Starts the selected timer
*/
void enable_timer(Timer timer)
{
  if (timer == TIMER0) LPC_TIM0->TCR = 1;
  else if (timer == TIMER1) LPC_TIM1->TCR = 1;
  else if (timer == TIMER2) LPC_TIM2->TCR = 1;
  else if (timer == TIMER3) LPC_TIM3->TCR = 1;
}


/**
 * Stops the selected timer
*/
void disable_timer(Timer timer)
{
  if (timer == TIMER0) LPC_TIM0->TCR = 0;
  else if (timer == TIMER1) LPC_TIM1->TCR = 0;
  else if (timer == TIMER2) LPC_TIM2->TCR = 0;
  else if (timer == TIMER3) LPC_TIM3->TCR = 0;
}


/**
 * Resets the selected timer
*/
void reset_timer(Timer timer)
{
  uint32_t regVal;

  if (timer == TIMER0)
  {
    regVal = LPC_TIM0->TCR;
    regVal |= 0x02;
    LPC_TIM0->TCR = regVal;
  }
  else if (timer == TIMER1)
  {
    regVal = LPC_TIM1->TCR;
    regVal |= 0x02;
    LPC_TIM1->TCR = regVal;
  }
  else if (timer == TIMER2)
  {
    regVal = LPC_TIM2->TCR;
    regVal |= 0x02;
    LPC_TIM2->TCR = regVal;
  }
  else if (timer == TIMER3)
  {
    regVal = LPC_TIM3->TCR;
    regVal |= 0x02;
    LPC_TIM3->TCR = regVal;
  }
}
