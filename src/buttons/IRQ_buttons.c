#include "buttons.h"
#include "lpc17xx.h"
#include "../timers/timer.h"
#include "../RIT/RIT.h"

extern int int0_down;
extern int key1_down;
extern int key2_down;

/****************/
/***** INT0 *****/
/****************/
void EINT0_IRQHandler(void)
{
	// Handle debouncing
	NVIC_DisableIRQ(EINT0_IRQn);			// Disable KEY0 interrupt
	LPC_PINCON->PINSEL4 &= ~(1 << 20);		// Disable KEY0 pressing
	
	int0_down = 1;
	
	LPC_SC->EXTINT &= (1 << 0);    			// clear pending interrupt
}



/****************/
/***** KEY1 *****/
/****************/
void EINT1_IRQHandler(void)
{
	NVIC_DisableIRQ(EINT1_IRQn);		// Disable KEY1 interrupt
	LPC_PINCON->PINSEL4 &= ~(1 << 22);	// Disable KEY1 pressing
	
	key1_down = 1;
	
	LPC_SC->EXTINT &= (1 << 1);   		// clear pending interrupt
}



/****************/
/***** KEY2 *****/
/****************/
void EINT2_IRQHandler(void)
{
	NVIC_DisableIRQ(EINT2_IRQn);		// Disable KEY2 interrupt
	LPC_PINCON->PINSEL4 &= ~(1 << 24);	// Disable KEY2 pressing
	
	key2_down = 1;
	
	LPC_SC->EXTINT &= (1 << 2);			// clear pending interrupt 
}
