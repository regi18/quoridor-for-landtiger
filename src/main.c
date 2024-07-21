/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC17xx.h"
#include "../GLCD/GLCD.h" 
#include "./timers/timer.h"
#include "./utils/LCD_utils.h"
#include "./utils/structs.h"
#include "./RIT/RIT.h"
#include "./buttons/buttons.h"
#include "./joystick/joystick.h"
#include "./utils/timers_utils.h"
#include "./CAN/CAN.h"


// Defined in system_LPC17xx.c !!
// #define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


int main(void)
{
	int i;

	SystemInit();  											// System Initialization (i.e., PLL)
	BUTTON_init();
	joystick_init();

	CAN_Init();

	// Init RIT to 50ms and starts it
	init_RIT(50);
	enable_RIT();
	
	// Use TIMER0 for game turn timing. Set to 1s (1000ms)
	// Set timer to trigger IRQ and restart on MR0 (but not stop, i.e. it will go on forever)
	init_timer(TIMER0, 1000, MCR_INTERRUPT_AND_RESET_ON_MR0);

	// init screen and draw the game grid
	LCD_init();
	LCD_draw_game_grid();

	// Init scoreboard
	LCD_scoreboard_init();

	// Draw greeting msg
	for (i = 1; i < 60; i++) { LCD_DrawLine(30, (MAX_Y - 135) / 2 + i, 210, (MAX_Y - 135) / 2 + i, White); } // Draw white rectangle
	GUI_Text((MAX_X - 60) / 2, (MAX_Y - 120) / 2, (uint8_t*)"WELCOME", Red, White);
	GUI_Text(45, (MAX_Y - 70) / 2, (uint8_t*)"PRESS INT0 TO START", Red, White);

		
	LPC_SC->PCON |= 0x1;									/* power-down	mode  */
	LPC_SC->PCON &= ~(0x2);						
	
	while (1)	{
		__ASM("wfi");
	}
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
