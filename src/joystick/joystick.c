/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           joystick.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        Atomic joystick init functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "joystick.h"


/*----------------------------------------------------------------------------
  Function that initializes joysticks and switch them off
 *----------------------------------------------------------------------------*/

void joystick_init(void) {
	/* joystick up functionality */
  LPC_PINCON->PINSEL3 &= ~(3<<26);	//PIN mode GPIO (00b value per P1.29)
	LPC_GPIO1->FIODIR   &= ~(1<<29);	//P1.29 Input (joysticks on PORT1 defined as Input) 
	
	// Joystick right functionality for P1.28
	LPC_PINCON->PINSEL3 &= ~(3 << 24);    // PIN mode GPIO (00b value per P1.28)
	LPC_GPIO1->FIODIR &= ~(1 << 28);    // P1.28 Input	

	// Joystick left functionality for P1.27
	LPC_PINCON->PINSEL3 &= ~(3 << 22);    // PIN mode GPIO (00b value per P1.27)
	LPC_GPIO1->FIODIR &= ~(1 << 27);    // P1.27 Input	
	
	// Joystick down functionality for P1.26
	LPC_PINCON->PINSEL3 &= ~(3 << 20);    // PIN mode GPIO (00b value per P1.26)
	LPC_GPIO1->FIODIR &= ~(1 << 26);    // P1.26 Input

	// Joystick select functionality for P1.25
	LPC_PINCON->PINSEL3 &= ~(3 << 18);    // PIN mode GPIO (00b value per P1.25)
	LPC_GPIO1->FIODIR &= ~(1 << 25);    // P1.25 Input
}



/************************************************************************************************/
/*********************************** HANDLE JOYSTICK ********************************************/
/************************************************************************************************/


/****************/
/**** UP LEFT ****/
/****************/
void JOY_UL_Callback(){
	if (is_movement_mode){
		// Movement mode
		JOY_Handle_Movement_Mode(UP_LEFT);	
	}
	
}


/****************/
/*** UP RIGHT ***/
/****************/
void JOY_UR_Callback(){
	if (is_movement_mode){
		// Movement mode
		JOY_Handle_Movement_Mode(UP_RIGHT);	
	}
}


/****************/
/** DOWN LEFT **/
/****************/
void JOY_DL_Callback(){
	if (is_movement_mode){
		// Movement mode
		JOY_Handle_Movement_Mode(DOWN_LEFT);	
	}
}


/****************/
/** DOWN RIGHT **/
/****************/
void JOY_DR_Callback(){
	if (is_movement_mode){
		// Movement mode
		JOY_Handle_Movement_Mode(DOWN_RIGHT);	
	}
}

/****************/
/**** SELECT ****/
/****************/
void JOYSEL_Callback(void) {
	Point2D p;
	MatrixPoint matrix13;
	PlayerStatus *curr_player_status; //, *other_player_status;
	
	// If moving player
	if(is_movement_mode && is_game_started){
		
		// Skip if player didn't change position
		// I.e. you press select without actually moving the player
		if(wanted_movement == NONE) return;

		// PLAYER 1 TURN
		if(is_player1_turn) {
			curr_player_status = &player1_status;
			// other_player_status = &player2_status;
		}
		// PLAYER 2 TURN
		else {
			curr_player_status = &player2_status;
			// other_player_status = &player1_status;
		}

		// clear the enable movements -> call LCD_draw_enable_movements with SCREEN_BACKGROUND_COLOR
		LCD_draw_enable_movements(curr_player_status->curr_pos.i, curr_player_status->curr_pos.j, SCREEN_BACKGROUND_COLOR);

		// clear player position
		matrix13 = get_index_13matrix_from_7matrix(curr_player_status->curr_pos.i, curr_player_status->curr_pos.j);
		p = get_coordinates_from_indexes(matrix13.i, matrix13.j);
		LCD_color_square(p.x, p.y, SCREEN_BACKGROUND_COLOR);

		// clear player position in the matrix
		remove_player_matrix(curr_player_status->curr_pos.i, curr_player_status->curr_pos.j);

		// update player current position to wanted_movement_matrix_position
		curr_player_status->curr_pos = wanted_movement_matrix_position;

		// color new player position
		matrix13 = get_index_13matrix_from_7matrix(curr_player_status->curr_pos.i, curr_player_status->curr_pos.j);
		p = get_coordinates_from_indexes(matrix13.i, matrix13.j);
		if(is_player1_turn) LCD_draw_pawn(p.x, p.y, PLAYER1_COLOR);
		else LCD_draw_pawn(p.x, p.y, PLAYER2_COLOR);

		// update game matrix
		set_game_matrix(is_player1_turn ? 1 : 2, matrix13.i, matrix13.j);

		// Check the victory
		if (has_won(*curr_player_status))
		{
			draw_victory_msg(is_player1_turn ? 1 : 2);
			is_game_started = false;
		}
		else
		{
			// curr_player hasn't won, now it's other_player turn
			set_player_turn(is_player1_turn ? 2 : 1);
		}
		
		wanted_movement = NONE;

		// Move confirmed, save it.
		save_player_move(matrix13);
	}
	// Else if moving wall
	else if(!is_movement_mode && is_game_started){
		//Wall movement -> select is used to confirm wall position
		matrix13.i = tmp_wall_position.i;
		matrix13.j = tmp_wall_position.j;
		
		//check if you can place wall there		
		if(is_wall_placement_possible(matrix13)){
			LCD_draw_wall(matrix13.i, matrix13.j, WALL_COLOR);
			place_wall(matrix13); //place and save wall inside the matrix
			
			//check which player is playing and remove one of their walls; update the scoreboard
			if(is_player1_turn){
				player1_status.walls_left-=1;
				LCD_scoreboard_update_walls_left(true, player1_status.walls_left);
			}
			else{
				player2_status.walls_left -=1;
				LCD_scoreboard_update_walls_left(false, player2_status.walls_left);
			}
			
			set_player_turn(is_player1_turn ? 2 : 1);
			is_movement_mode = true;
		
			// Move confirmed, save it.
			if (matrix13.i%2 == 1 && matrix13.j%2 == 0) save_wall_move(true, matrix13); //Horizontal wall
			else if(matrix13.i%2 == 0 && matrix13.j%2 == 1) save_wall_move(false, matrix13); //Vertical wall
		}
		else {
			LCD_draw_message((uint8_t *) "INVALID WALL POSITION");
		}
	}
	//we are still in the initial menu
	else if(!is_game_started && is_game_menu){ 
		//check if we are choosing the board or the type of player, and then start the game depending on the choices made
		if(is_board_choice){
				if(is_single_board) LCD_player_menu();
				else {
					if(remote_board_status == AVAILABLE) LCD_player_menu();
					else return;
				}
				is_board_choice = false;
		}
		else {
			if (is_single_board) start_game(); 
			else {
				CAN_send_current_board_ready();
				GUI_Text(MENU_SIDE_PAD, MAX_Y - MENU_SPACE_BETWEEN, (uint8_t*)"WAITING FOR OTHER", GRID_BORDER_COLOR, SCREEN_BACKGROUND_COLOR);

				if (other_board_ready) start_game(); 
			}
		}
	}
}


/****************/
/**** DOWN ******/
/****************/
void JOYDOWN_Callback(void) {
	
	if (is_movement_mode && is_game_started){
		// Movement mode
		JOY_Handle_Movement_Mode(DOWN);	
	}
	else if(!is_movement_mode && is_game_started){	
		//move wall down
		JOY_handle_wall_movement(DOWN);                         
	} 
	else if(!is_game_started && is_game_menu){
		//check if we are choosing the board or the type of player
		if(is_board_choice){
			//color the bottom choice "two boards"
			is_single_board = false;
			LCD_game_mode();
		} else {		
			//set human = false
			is_human = false;
			LCD_player_menu();
		}
		
	}

}



/****************/
/****** UP ******/
/****************/

void JOYUP_Callback(void) {
	
	if (is_movement_mode && is_game_started){
		// Movement mode
		JOY_Handle_Movement_Mode(UP);	
	}
	else if (!is_movement_mode && is_game_started){
		//move wall up
		JOY_handle_wall_movement(UP); 
	}
	else if(!is_game_started && is_game_menu){
		//check if we are choosing the board or the type of player
		if(is_board_choice){
			//color the upper choice "single board"
			is_single_board = true;
			LCD_game_mode();
		} else {		
			//set human = false
			is_human = true;
			LCD_player_menu();
		}
		
	}
}



/****************/
/***** LEFT *****/
/****************/
void JOYLEFT_Callback(void) {
	
	if (is_movement_mode){
		// Movement mode
		JOY_Handle_Movement_Mode(LEFT);	
	}
	else{	
		//move wall left
		JOY_handle_wall_movement(LEFT);
	}
}


/****************/
/**** RIGHT *****/
/****************/
void JOYRIGHT_Callback(void) {
	
	if (is_movement_mode){
		// Movement mode
		JOY_Handle_Movement_Mode(RIGHT);	
	}
	else{	
		//move wall right
		JOY_handle_wall_movement(RIGHT);
	}
}


void JOY_Handle_Movement_Mode(Movement move){
	MatrixPoint curr_pos_player; //position in 7x7 matrix
	MatrixPoint matrix_pos_player; //position in 13x13 matrix
	MatrixPoint matrix13_wanted_position;
	Point2D p;
	bool check = false;
	bool jump_other_player = false;

	
	//take 7x7 position
	if(is_player1_turn)
		curr_pos_player = player1_status.curr_pos;
	else
		curr_pos_player = player2_status.curr_pos;
	
	// take 13x13 position
	matrix_pos_player = get_index_13matrix_from_7matrix(curr_pos_player.i, curr_pos_player.j);
	
	//check_movement 
	if(move == UP){
		if(check_movement(matrix_pos_player.i, matrix_pos_player.j, UP)){
			//move up possible
			//Check that the other player isn't already there
			if(check_other_player(matrix_pos_player.i, matrix_pos_player.j, UP)){
				//check if the player can jump the other player
				check = check_movement(matrix_pos_player.i - 2, matrix_pos_player.j, UP);
				//save if player has to jump the other player or not
				jump_other_player = check;
			}
			else
				check = true;
		}
		else
			//move not possible
			check = false;
	}
	else if(move == DOWN){
		if(check_movement(matrix_pos_player.i, matrix_pos_player.j, DOWN)){
			//move down possible
			//Check that the other player isn't already there
			if(check_other_player(matrix_pos_player.i, matrix_pos_player.j, DOWN)){
				//check if the player can jump the other player
				check = check_movement(matrix_pos_player.i + 2, matrix_pos_player.j, DOWN);
				//save if player has to jump the other player or not
				jump_other_player = check;
			}
			else
				check = true;
		}
		else
			//move not possible
			check = false;
	}
	else if(move == RIGHT){
		if(check_movement(matrix_pos_player.i, matrix_pos_player.j, RIGHT)){
			//move right possible
			//Check that the other player isn't already there
			if(check_other_player(matrix_pos_player.i, matrix_pos_player.j, RIGHT)){
				//check if the player can jump the other player
				check = check_movement(matrix_pos_player.i, matrix_pos_player.j + 2, RIGHT);
				//save if player has to jump the other player or not
				jump_other_player = check;
			}
			else
				check = true;
		}
		else
			//move right not possible
			check = false;
	}
	else if(move == LEFT){
		if(check_movement(matrix_pos_player.i, matrix_pos_player.j, LEFT)){
			//move left possible
			//Check that the other player isn't already there
			if(check_other_player(matrix_pos_player.i, matrix_pos_player.j, LEFT)){
				//check if the player can jump the other player
				check = check_movement(matrix_pos_player.i, matrix_pos_player.j+2, LEFT);
				//save if player has to jump the other player or not
				jump_other_player = check;
			}
			else
				check = true;
		}
		else
			//move left not possible
			check = false;
	}
	else if(move == UP_RIGHT) 
		check = check_oblique_movement(matrix_pos_player.i, matrix_pos_player.j, UP_RIGHT);
	else if(move == UP_LEFT)
		check = check_oblique_movement(matrix_pos_player.i, matrix_pos_player.j, UP_LEFT);
	else if(move == DOWN_RIGHT)
		check = check_oblique_movement(matrix_pos_player.i, matrix_pos_player.j, DOWN_RIGHT);
	else if(move == DOWN_LEFT)
		check = check_oblique_movement(matrix_pos_player.i, matrix_pos_player.j, DOWN_LEFT);
	
	
	if(check){
		//move is possible
		
		//check if wanted_movement is not none
		if(wanted_movement != NONE){
			//this isn't the first wanted movement of the player -> restore original square color of the ex wanted movement
			//get 13x13 indexes
			matrix13_wanted_position = get_index_13matrix_from_7matrix(wanted_movement_matrix_position.i, wanted_movement_matrix_position.j);
			//get coordinates
			p = get_coordinates_from_indexes(matrix13_wanted_position.i, matrix13_wanted_position.j);
			//restore the color
			LCD_color_square(p.x, p.y, ENABLE_MOVEMENTS_COLOR);
		}
		
		//set wanted_movement
		if(move == UP){
			wanted_movement = UP;
			//check if the player has to jump the other player; if yes, player wanted_movement is two times up; if no, player wanted_movement is up
			if (jump_other_player)
				wanted_movement_matrix_position.i = curr_pos_player.i - 2;
			else
				wanted_movement_matrix_position.i = curr_pos_player.i - 1;
			wanted_movement_matrix_position.j = curr_pos_player.j;
		}
		else if(move == DOWN){
			wanted_movement = DOWN;
			//check if the player has to jump the other player; if yes, player wanted_movement is two times down; if no, player wanted_movement is down
			if(jump_other_player)
				wanted_movement_matrix_position.i = curr_pos_player.i + 2;
			else
				wanted_movement_matrix_position.i = curr_pos_player.i + 1;
			wanted_movement_matrix_position.j = curr_pos_player.j;
		}
		else if(move == RIGHT){
			wanted_movement = RIGHT;
			//check if the player has to jump the other player; if yes, player wanted_movement is two times right; if no, player wanted_movement is right
			wanted_movement_matrix_position.i = curr_pos_player.i;
			if(jump_other_player)
				wanted_movement_matrix_position.j = curr_pos_player.j+2;
			else
				wanted_movement_matrix_position.j = curr_pos_player.j+1;
		}
		else if(move == LEFT){
			wanted_movement = LEFT;
			wanted_movement_matrix_position.i = curr_pos_player.i;
			if(jump_other_player)
				wanted_movement_matrix_position.j = curr_pos_player.j-2;
			else
				wanted_movement_matrix_position.j = curr_pos_player.j-1;
		}
		else if(move == UP_LEFT){
			wanted_movement = UP_LEFT;
			wanted_movement_matrix_position.i = curr_pos_player.i-1;
			wanted_movement_matrix_position.j = curr_pos_player.j-1;
		}
		else if(move == UP_RIGHT){
			wanted_movement = UP_RIGHT;
			wanted_movement_matrix_position.i = curr_pos_player.i-1;
			wanted_movement_matrix_position.j = curr_pos_player.j+1;
		}
		else if(move == DOWN_LEFT){
			wanted_movement = UP_LEFT;
			wanted_movement_matrix_position.i = curr_pos_player.i+1;
			wanted_movement_matrix_position.j = curr_pos_player.j-1;
		}
		else if(move == DOWN_RIGHT){
			wanted_movement = UP_LEFT;
			wanted_movement_matrix_position.i = curr_pos_player.i+1;
			wanted_movement_matrix_position.j = curr_pos_player.j+1;
		}
			

		//get 13x13 indexes
		matrix13_wanted_position = get_index_13matrix_from_7matrix(wanted_movement_matrix_position.i, wanted_movement_matrix_position.j);
		//get coordinates
		p = get_coordinates_from_indexes(matrix13_wanted_position.i, matrix13_wanted_position.j);
		//color (Magenta) the wanted movement
		LCD_color_square(p.x, p.y, WANTED_MOVEMENT_COLOR);
	}
	else{
		//move not possible
		//TODO: print "error: move not avaiable"
	}
}



void JOY_handle_wall_movement(Movement move){
	MatrixPoint check_next_wall_position;
	
	if(move == DOWN){
		check_next_wall_position.i = tmp_wall_position.i + 2;
		check_next_wall_position.j = tmp_wall_position.j;
		
		if (check_next_wall_position.i >= 12) {
			LCD_draw_message((uint8_t *)"Outside grid"); 			//print error message
			return;
		}
		
		LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, SCREEN_BACKGROUND_COLOR);			
		LCD_redraw_all_walls();
		tmp_wall_position.i += 2;
		LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, is_wall_placement_possible(tmp_wall_position) ? POSSIBLE_WALL_POSITION_COLOR : IMPOSSIBLE_WALL_POSITIN_COLOR);
	}
	
	if(move == UP){
		check_next_wall_position.i = tmp_wall_position.i - 2;
		check_next_wall_position.j = tmp_wall_position.j;
		
		if (check_next_wall_position.i < 0) {
			LCD_draw_message((uint8_t *)"Outside grid"); 			//print error message
			return;
		}
		
		LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, SCREEN_BACKGROUND_COLOR);			
		LCD_redraw_all_walls();
		tmp_wall_position.i -= 2;
		LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, is_wall_placement_possible(tmp_wall_position) ? POSSIBLE_WALL_POSITION_COLOR : IMPOSSIBLE_WALL_POSITIN_COLOR);
	}
	
	if(move == LEFT){
		check_next_wall_position.i = tmp_wall_position.i;
		check_next_wall_position.j = tmp_wall_position.j - 2;
		
		if (check_next_wall_position.j < 0) {
			LCD_draw_message((uint8_t *)"Outside grid"); 			//print error message
			return;
		}
		
		LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, SCREEN_BACKGROUND_COLOR);			
		LCD_redraw_all_walls();
		tmp_wall_position.j -= 2;
		LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, is_wall_placement_possible(tmp_wall_position) ? POSSIBLE_WALL_POSITION_COLOR : IMPOSSIBLE_WALL_POSITIN_COLOR);
	}
	
	if(move == RIGHT){
		check_next_wall_position.i = tmp_wall_position.i;
		check_next_wall_position.j = tmp_wall_position.j + 2;
		
		if (check_next_wall_position.j >= 12) {
			LCD_draw_message((uint8_t *)"Outside grid"); 			//print error message
			return;
		}
		
		LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, SCREEN_BACKGROUND_COLOR);			
		LCD_redraw_all_walls();
		tmp_wall_position.j += 2;
		LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, is_wall_placement_possible(tmp_wall_position) ? POSSIBLE_WALL_POSITION_COLOR : IMPOSSIBLE_WALL_POSITIN_COLOR);
	}	
}

