#include "lpc17xx.h"
#include "RIT.h"

void enable_RIT(void)
{
	LPC_RIT->RICTRL |= (1 << 3);
}

void disable_RIT(void)
{
	LPC_RIT->RICTRL &= ~(1 << 3);
}

void reset_RIT(void)
{
	LPC_RIT->RICOUNTER = 0; // Set count value to 0
	return;
}

void init_RIT(uint32_t ms)
{
	LPC_SC->PCLKSEL1 &= ~(3 << 26);
	LPC_SC->PCLKSEL1 |= (1 << 26); // RIT Clock = CCLK
	LPC_SC->PCONP |= (1 << 16);	   // Enable power for RIT

	LPC_RIT->RICOMPVAL = get_match_for_timer(RIT, ms); // Set match register
	LPC_RIT->RICTRL = (1 << 1) |	  // Enable clear on match
					  (1 << 2);		  // Enable timer for debug (The timer is halted when the processor is halted for debugging)
	LPC_RIT->RICOUNTER = 0;			  // Set count value to 0

	NVIC_EnableIRQ(RIT_IRQn);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
