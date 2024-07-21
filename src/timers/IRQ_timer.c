#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../utils/LCD_utils.h"
#include "../utils/game_utils.h"
#include "../CAN/CAN.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

/******************/
/***** TIMER0 *****/
/******************/
void TIMER0_IRQHandler (void) {
	// TIMER0 will decrease the counter every second, and if it reaches 0 then turn ends.
	turn_time_left--;
	
	// Times up, restart timer and switch player
	if(turn_time_left == 0) {
		// Turn finished before making any move, i.e. the player skipped the move.
		// Save the skipped move only if it's my turn and there are 2 boards
		if (!is_single_board && is_game_started && ((is_player1_turn && this_board_player_id == PLAYER1) || (!is_player1_turn && this_board_player_id == PLAYER2)))
			save_skipped_move();

		// Remove unset wall (if present)
		LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, SCREEN_BACKGROUND_COLOR);
		LCD_redraw_all_walls();

		// Change player, the other finished
		if (is_player1_turn) set_player_turn(2); 
		else set_player_turn(1); 
	}
	else {
		// Update time on scoreboard
		LCD_scoreboard_update_time(turn_time_left);
	}
	
	LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}



/******************/
/***** TIMER1 *****/
/******************/
void TIMER1_IRQHandler (void)
{
	int i = 0;
	uint16_t y0 = GRID_SIZE * SQUARE_LENGTH + (GRID_SIZE - 1) * WALL_SPACE + GRID_PADDING + 2;  // starting Y-coordinate, to increment
	
	for(i = 0; i < 15; i++){
		LCD_DrawLine(0, y0, 240, y0, SCREEN_BACKGROUND_COLOR);
		y0++;
	}
	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}




/******************/
/***** TIMER2 *****/
/******************/
void TIMER2_IRQHandler (void)
{
  	LPC_TIM2->IR = 1;			/* clear interrupt flag */
}




/******************/
/***** TIMER3 *****/
/******************/
void TIMER3_IRQHandler (void)
{
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
}




/******************************************************************************
* 	                             End Of File
******************************************************************************/
