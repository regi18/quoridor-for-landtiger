#include "buttons.h"

/**
 * @brief  Function that initializes Buttons
 */
void BUTTON_init(void)
{

	LPC_PINCON->PINSEL4 |= (1 << 20); /* External interrupt 0 pin selection */
	LPC_GPIO2->FIODIR &= ~(1 << 10);  /* PORT2.10 defined as input          */

	LPC_PINCON->PINSEL4 |= (1 << 22); /* External interrupt 0 pin selection */
	LPC_GPIO2->FIODIR &= ~(1 << 11);  /* PORT2.11 defined as input          */

	LPC_PINCON->PINSEL4 |= (1 << 24); /* External interrupt 0 pin selection */
	LPC_GPIO2->FIODIR &= ~(1 << 12);  /* PORT2.12 defined as input          */

	LPC_SC->EXTMODE = 0x7; /* Edge sensitivity selected for !EINT0,1,2 */
						   /* Since EXTPOLAR is not set (defaults to 0), !EINT is falling-edge sensitive */

	NVIC_EnableIRQ(EINT2_IRQn);		 /* enable irq in nvic                 */
	NVIC_SetPriority(EINT2_IRQn, 3); /* priority, the lower the better     */

	NVIC_EnableIRQ(EINT1_IRQn); /* enable irq in nvic                 */
	NVIC_SetPriority(EINT1_IRQn, 2);

	NVIC_EnableIRQ(EINT0_IRQn);		 /* enable irq in nvic                 */
	NVIC_SetPriority(EINT0_IRQn, 1); /* decreasing priority	from EINT2->0	 */
}

/***************************************************/
/*                    CALLBACKS                    */
/***************************************************/

#include "../utils/LCD_utils.h"
#include "../utils/structs.h"
#include "../utils/game_utils.h"
#include "../utils/CAN_utils.h"
#include "../timers/timer.h"


/****************/
/***** INT0 *****/
/****************/
void INT0_Callback(void)
{
	// Send handshake. This also decides the board's player_id
	CAN_send_handshake();
	
	//Init menu 
	LCD_game_mode();
}


/****************/
/***** KEY1 *****/
/****************/
void KEY1_Callback(void)
{
	MatrixPoint position;

	// Ignore if game still not started
	if (!is_game_started) return;

	if(is_player1_turn) position = player1_status.curr_pos;
	else position = player2_status.curr_pos;
	
	// Check if the player has any walls left
	if ((is_player1_turn && player1_status.walls_left > 0) || (!is_player1_turn && player2_status.walls_left > 0))
	{
			// If walls > 0, toggle between movement and wall placement
			is_movement_mode = !is_movement_mode;

			// Switch between placing wall and moving player
			LCD_draw_enable_movements(position.i, position.j, is_movement_mode ? ENABLE_MOVEMENTS_COLOR : SCREEN_BACKGROUND_COLOR);

			LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, SCREEN_BACKGROUND_COLOR);
			LCD_redraw_all_walls();

			if (!is_movement_mode)
			{
				tmp_wall_position.i = 6;
				tmp_wall_position.j = 5;
				LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, is_wall_placement_possible(tmp_wall_position) ? POSSIBLE_WALL_POSITION_COLOR : IMPOSSIBLE_WALL_POSITIN_COLOR); // draw default wall in the center
				is_wall_horizontal = false;
				// show text, it will clear after the player moves the wall
				LCD_draw_message((uint8_t *)"USE JOYSTICK TO MOVE WALL");
			}
	}
	else
	{
		// If walls = 0, player can only move
		is_movement_mode = true; // Force movement mode
		LCD_draw_message((uint8_t *)"NO WALLS AVAILABLE");
		LCD_draw_enable_movements(position.i, position.j, ENABLE_MOVEMENTS_COLOR);
	}
}




/****************/
/***** KEY2 *****/
/****************/
void KEY2_Callback(void) { 
	// Skip if not placing wall
	if (is_movement_mode) return;

	rotate_wall();
}




/****************/
/***** INT3 *****/
/****************/
void EINT3_Callback(void) {}
