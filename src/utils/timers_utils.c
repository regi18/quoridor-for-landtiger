#include "timers_utils.h"


uint32_t get_match_for_timer(TimerType timer, uint32_t ms) {
    uint8_t state;
    uint32_t pclk;
	
    // For Timer0 bits[3:2] in PCLKSEL0 are used.
    // For Timer1 bits[5:4] in PCLKSEL0 are used.
    // For Timer2 bits[13:12] in PCLKSEL1 are used.
    // For Timer3 bits[15:14] in PCLKSEL1 are used.
    // For RIT bits[27:26] in PCLKSEL1 are used.
    if (timer == TIM0) state = (LPC_SC->PCLKSEL0 >> 2) & 3;
    else if (timer == TIM1) state = (LPC_SC->PCLKSEL0 >> 4) & 3;
    else if (timer == TIM2) state = (LPC_SC->PCLKSEL1 >> 12) & 3;
    else if (timer == TIM3) state = (LPC_SC->PCLKSEL1 >> 14) & 3;
    else if (timer == RIT) state = (LPC_SC->PCLKSEL1 >> 26) & 3;

    // Get the timer's clock
    if (state == 0) pclk = SystemFrequency/4;
    else if (state == 0x01) pclk = SystemFrequency;
    else if (state == 0x02) pclk = SystemFrequency/2;
    else if (state == 0x03) pclk = SystemFrequency/8;

    // count = frequency * time (seconds = ms/1000)
    return pclk * ((double)ms / 1000);
}
