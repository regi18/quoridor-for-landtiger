/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"


volatile int int0_down = 0;
volatile int key1_down = 0;
volatile int key2_down = 0;

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
******************************************************************************/
void RIT_IRQHandler(void)
{
	/********************/
	/***** JOYSTICK *****/
	/********************/
	static int j_select=0; //LPC_GPI01 bit 25
	static int j_down=0;	//bit 26
	static int j_left=0;	//bit 27
	static int j_right=0;	//bit 28
	static int j_up=0;		//bit 29
	
	static int j_UL = 0;	//variable to control UP-LEFT
	static int j_UR = 0;	//variable to control UP-RIGHT
	static int j_DL = 0;	//variable to control DOWN-LEFT
	static int j_DR = 0;	//variable to control DOWN-RIGHT
	
	// Ignore actions if it is opponent turn (2 boards)
	if (!is_single_board && is_game_started && ((is_player1_turn && this_board_player_id == PLAYER2) || (!is_player1_turn && this_board_player_id == PLAYER1)))
		return;
	
	is_double_movement_mode = false;
	
	//check first if two joystick keys are pressed at the same time
	if((LPC_GPIO1->FIOPIN & (10<<26)) == 0 ){ /* Joystick UP and LEFT are pressed*/
		is_double_movement_mode = true;
		if(j_UL == 1) 
			JOY_UL_Callback();
		j_UL++;
	} else { j_UL = 0; }
	
	if((LPC_GPIO1->FIOPIN & (12<<26)) == 0 ){ /* Joystick UP and RIGHT are pressed*/
		is_double_movement_mode = true;
		if(j_UR == 1) 
			JOY_UR_Callback();
		j_UR++;
	} else { j_UR = 0; }
	
	if((LPC_GPIO1->FIOPIN & (3<<26)) == 0 ){ /* Joystick DOWN and LEFT are pressed*/
		is_double_movement_mode = true;
		if(j_DL == 1)
			JOY_DL_Callback();
		j_DL++;
	} else { j_DL = 0; }
	
	if((LPC_GPIO1->FIOPIN & (5<<26)) == 0 ){ /* Joystick DOWN and RIGHT are pressed*/
		is_double_movement_mode = true;
		if(j_DR == 1)
			JOY_DR_Callback();
		j_DR++;
	} else { j_DR = 0; }
	
	//check if one joystick key is pressed
	if ((LPC_GPIO1->FIOPIN & (1<<25)) == 0 ) { /* Joystick Select pressed */
		is_double_movement_mode = false;
		if(j_select == 1) 
			JOYSEL_Callback(); 
		j_select++;
	}	else { j_select = 0; }
	
	if ((LPC_GPIO1->FIOPIN & (1<<26)) == 0 ) { /* Joystick Down pressed */
		if(!is_double_movement_mode){
			if(j_down == 1) 
				JOYDOWN_Callback();
			j_down++;
		}
	} else { j_down = 0; }
	
	if ((LPC_GPIO1->FIOPIN & (1<<27)) == 0 ) { /* Joystick Left pressed */
		if(!is_double_movement_mode){
			if(j_left == 1) 
				JOYLEFT_Callback();
			j_left++;
		}
	} else { j_left = 0; }
	
	if ((LPC_GPIO1->FIOPIN & (1<<28)) == 0 ) { /* Joystick Right pressed */
		if(!is_double_movement_mode){
			if(j_right == 1) 
				JOYRIGHT_Callback();
			j_right++;
		}
	} else { j_right = 0; }
	
	if ((LPC_GPIO1->FIOPIN & (1<<29)) == 0 ) { /* Joystick Up pressed */
		if(!is_double_movement_mode){
			if(j_up == 1)  
				JOYUP_Callback();
			j_up++;
		}
	} else { j_up = 0; }

	/****************/
	/***** INT0 *****/
	/****************/
	if (int0_down != 0) { 
		// Button INT0 pressed
		if ((LPC_GPIO2->FIOPIN & (1 << 10)) == 0) {
			if (int0_down == 1) INT0_Callback();
			int0_down++;
		}
		// button INT0 released
		else{
			int0_down = 0;
			NVIC_EnableIRQ(EINT0_IRQn);		  // Enable KEY0 interrupt
			LPC_PINCON->PINSEL4 |= (1 << 20); // Enable KEY0 pressing
		}
	}

	/****************/
	/***** KEY1 *****/
	/****************/
	if (key1_down != 0)	{ 
		// Button KEY1 pressed
		if ((LPC_GPIO2->FIOPIN & (1 << 11)) == 0){
			if (key1_down == 1) KEY1_Callback(); //if KEY1 is pressed draw a wall in the center of the board
			key1_down++;
		}
		// Button KEY1 released
		else { 
			key1_down = 0;
			NVIC_EnableIRQ(EINT1_IRQn);		  // Enable KEY1 interrupt
			LPC_PINCON->PINSEL4 |= (1 << 22); // Enable KEY1 pressing
		}
	}

	/****************/
	/***** KEY2 *****/
	/****************/
	if (key2_down != 0)	{
		// Button KEY2 pressed
		if ((LPC_GPIO2->FIOPIN & (1 << 12)) == 0)		{ 
			if (key2_down == 1) KEY2_Callback();
			key2_down++;
		}
 		// Button KEY2 released
		else	{
			key2_down = 0;
			NVIC_EnableIRQ(EINT2_IRQn);		  // Enable KEY2 interrupt
			LPC_PINCON->PINSEL4 |= (1 << 24); // Enable KEY2 pressing
		}
	}

	LPC_RIT->RICTRL |= 0x1; /* clear interrupt flag */
	return;
}


/******************************************************************************
*                                End Of File
******************************************************************************/
