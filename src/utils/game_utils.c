/* Includes ------------------------------------------------------------------*/
#include "game_utils.h"
#include "../joystick/joystick.h"
#include "../timers/timer.h"

#include <stdio.h>
#include <stdlib.h>

int8_t game_matrix[13][13];

PlayerStatus player1_status;
PlayerStatus player2_status;

bool is_game_started = false;

uint8_t turn_time_left;

bool is_player1_turn;	// true -> player1 turn, false -> player2 turn

bool is_movement_mode;	// true -> player is moving, false -> player is placing a wall

bool is_wall_horizontal;	// true -> horizontal, false -> vertical

MatrixPoint tmp_wall_position;	// Coordinates of the wall that the user is placing

Movement wanted_movement;	// Place the temporary movement "preference" of the player, to be confirmed when clicking JOY SELECT

MatrixPoint wanted_movement_matrix_position; //wanted position indexes in 7x7 matrix

bool is_double_movement_mode = false; //true -> single move (UP, LEFT, RIGHT, DOWN), false -> double move (UP_RIGHT, UP_LEFT, ...)

bool visited[GRID_SIZE][GRID_SIZE];	// Used to check if a player is trapped using DFS

bool is_game_menu = false;
bool is_board_choice = true;
bool is_single_board = true;	// true -> game is running on a single board; false -> game running via CAN on 2 boards
bool is_human = true;
// If is_single_board && is_human -> Extrapoint 1: single board and alternating player turns
// If is_single_board && !is_human -> single board and opponent is NPC
// If !is_single_board && is_human -> Game runs over CAN, this board is controlled by a human
// If !is_single_board && !is_human -> Game runs over CAN, this boards is played by NPC


/* Functions ------------------------------------------------------------------*/


/**
 * Initialize the matrix: no walls, no players, -1 for impossible positions
*/
void init_game_matrix() {
	int i;
	int j;
	for(i=0; i<13; i++) {
		for(j=0; j<13; j++) {
			if (i%2 == 1 && j%2 == 1)
				//impossible position (crossing of walls)
				game_matrix[i][j] = -1;
			else
				game_matrix[i][j] = 0;
		}
	}
}

/**
 * set a player inside the matrix
 *
 * @param i, j matrix indexes 13x13
*/

void set_game_matrix(int player, int i, int j){
	game_matrix[i][j] = player;
}

/**
 * set a player inside the matrix
 *
 * @param i, j matrix indexes 13x13
*/

void remove_player_matrix(int i, int j){
	MatrixPoint p = get_index_13matrix_from_7matrix(i, j);
	game_matrix[p.i][p.j] = 0;
}

/**
 * Check if the player with the given status has won.
 *
 * Look inside the map to identify and locate the player, and check the coordinates to look for victory.
 *
 * @param status PlayerStatus of the player for which we're checking the victory
 */
bool has_won(PlayerStatus status) {
	// Get the real coordinates (convert from 7x7 to 13x13)
	MatrixPoint coordinates = get_index_13matrix_from_7matrix(status.curr_pos.i, status.curr_pos.j);

	// Get the value inside the given cell
	int cell = game_matrix[coordinates.i][coordinates.j];

	// Player is player 1, that has to reach the bottom
	if (cell == 1 && coordinates.i == 12) {
		disable_timer(TIMER0);
		return true;
	}

	// Player is player 2, that has to reach the top
	if (cell == 2 && coordinates.i == 0) {
		disable_timer(TIMER0);
		return true;
	}

	// No one has won yet
	return false;
}


/**
 * Draw victory message
 *
 * @param victory player
 */

void draw_victory_msg(int player) {
	int i;
	// Draw victory msg
	for (i = 1; i < 60; i++) { LCD_DrawLine(15, (MAX_Y - 135) / 2 + i, 225, (MAX_Y - 135) / 2 + i, White); } // Draw white rectangle

	if(player == 1)
		GUI_Text((MAX_X - 135) / 2, (MAX_Y - 120) / 2, (uint8_t*)"PLAYER 1 HAS WON!", Red, White);
	else
		GUI_Text((MAX_X - 135) / 2, (MAX_Y - 120) / 2, (uint8_t*)"PLAYER 2 HAS WON!", Red, White);

	GUI_Text(18, (MAX_Y - 70) / 2, (uint8_t*)"PRESS INT0 TO START AGAIN", Red, White);
}

/**
 * Get indexes of the 13x13 (game_matrix) matrix from the indexes of the 7x7 matrix (players matrix)
 *
 * @param i_7 i index of the 7x7 matrix
 * @param j_7 j index of the 7x7 matrix
 */
MatrixPoint get_index_13matrix_from_7matrix(int i_7, int j_7)
{
	// A valid cell has both coordinates even; in order to get the correct position I just need to double each coordinate.
	MatrixPoint p;
	p.i = i_7 * 2;
	p.j = j_7 * 2;
	return p;
}

/**
 * Get indexes of the 7x7 matrix (players matrix) from the indexes of the 13x13 matrix (game matrix)
 * 
 * @param i_13 i index of the 13x13 matrix 
 * @param j_13 j index of the 13x13 matrix
*/
MatrixPoint get_index_7matrix_from_13matrix(int i_13, int j_13) {
	// A valid cell has both coordinates even; in order to get the correct position I just need to double each coordinate.
	MatrixPoint p;
	p.i = i_13 / 2;
	p.j = j_13 / 2;
	return p;
}

/**
 * conversion from 13x13 matrix indexes to universal format indexes
 * 
 * @param i_13 i index of the game matrix 13x13
 * @param j_13 j index of the game matrix 13x13
 * 
 * @return MatrixPoint representing universal format indexes
*/
MatrixPoint get_universal_format_from_13matrix(int i_13, int j_13){
	MatrixPoint p_7, p_universal;

	if (i_13%2 == 0 && j_13%2 == 0){
		//token position
		//universal indexes are inverted with respect to 7x7 matrix indexes
		p_7 = get_index_7matrix_from_13matrix(i_13, j_13);
		p_universal.i = p_7.j;
		p_universal.j = p_7.i;
	}
	else if(i_13%2 == 0 && j_13%2 == 1){
		//vertical wall position
		//return the centre of the wall in position [i_13, j_13] and [i_13+1, j_13] (this square and the one below)
		//universal indexes are inverted with respect to matrix indexes
		j_13 = j_13 -1;
		p_7 = get_index_7matrix_from_13matrix(i_13, j_13);
		p_universal.i = p_7.j;
		p_universal.j = p_7.i;
	}
	else if(i_13%2 == 1 && j_13%2 == 0){
		//horizontal wall position
		//return the centre of the wall in position [i_13, j_13] and [i_13, j_13+1] (this square and the one to its right)
		//universal indexes are inverted with respect to matrix indexes
		i_13 = i_13 -1;
		p_7 = get_index_7matrix_from_13matrix(i_13, j_13);
		p_universal.i = p_7.j;
		p_universal.j = p_7.i;
	}
	else if(i_13%2 == 1 && j_13%2 == 1){
		//impossible position
	}
	return p_universal;
}



/**
 * conversion from universal format indexes to 13x13 matrix indexes
 * 
 * @param i i index of the coordinate system (universal format)
 * @param j j index of the coordinate system (universal format)
 * @param is_wall true if conversion of wall position (returned the indexes of the up/left square), false if conversion of token position
 * @param is_horizontal if conversion of wall position it's necessary the orientation of the wall (vertical -> returned up square, horizontal -> returned left square)
 * 
 * @return MatrixPoint representing 13x13 matrix indexes
*/
MatrixPoint get_13matrix_from_universal_format(int i, int j, bool is_wall, bool is_horizontal){
	MatrixPoint p_13;
	if(!is_wall){
		//token position
		//universal indexes are inverted with respect to 7x7 matrix indexes
		p_13 = get_index_13matrix_from_7matrix(j, i);
	}
	else{
		//wall position
		if(!is_horizontal){
			//wall is vertical
			//i and j are the indexes of the centre of the wall, return the indexes of the up square of the wall
			//universal indexes are inverted with respect to 7x7 matrix indexes
			p_13 = get_index_13matrix_from_7matrix(j, i);
			p_13.j += 1;
		}
		else{
			//wall is horizontal
			//i and j are the indexes of the centre of the wall, return the indexes of the left square of the wall
			//universal indexes are inverted with respect to 7x7 matrix indexes
			p_13 = get_index_13matrix_from_7matrix(j, i);
			p_13.i += 1;
		}
	}
	return p_13;
}


/**
 * Check if the move is possible; check only the adiacent walls and the position in the grid
 * 
 * @param i i index of the game matrix 13x13
 * @param j j index of the game matrix 13x13
 * @param move (UP = 0, etc..)
 * 
 * @return true if the move is possible, return false if it's not
*/
bool check_movement(int i, int j, Movement move)
{
	if (move == UP)
	{
		if (i > 0)
			return (game_matrix[i - 1][j] == 0);
		else
			return false;
	}
	if (move == RIGHT)
	{
		if (j < 12)
			return (game_matrix[i][j + 1] == 0);
		else
			return false;
	}
	if (move == DOWN)
	{
		if (i < 12)
			return (game_matrix[i + 1][j] == 0);
		else
			return false;
	}
	if (move == LEFT)
	{
		if (j > 0)
			return (game_matrix[i][j - 1] == 0);
		else
			return false;
	}
	return false;
}

/**
 * Check if there is the other player in move direction
 * 
 * @param i i index of the game matrix 13x13
 * @param j j index of the game matrix 13x13
 * @param move (UP = 0, etc..)
 * 
 * @return true if there is the player, false otherwise
*/
bool check_other_player(int i, int j, Movement move){
	if (move == UP)
	{
		if (i > 0)
			//if there is a player return true
			return (game_matrix[i - 2][j] != 0);
		else
			return false;
	}
	if (move == RIGHT)
	{
		if (j < 12)
			return (game_matrix[i][j + 2] != 0);
		else
			return false;
	}
	if (move == DOWN)
	{
		if (i < 12)
			return (game_matrix[i + 2][j] != 0);
		else
			return false;
	}
	if (move == LEFT)
	{
		if (j > 0)
			return (game_matrix[i][j - 2] != 0);
		else
			return false;
	}
	return false;
}

/**
* Check if the oblique move is possible 
 * 
 * @param i i index of the game matrix 13x13
 * @param j j index of the game matrix 13x13
 * @param move (UP_LEFT = 4, UP_RIGHT = 5, DOWN_LEFT = 6,DOWN_RIGHT = 7)
 * 
 * @return true if the move is possible, return false if it's not
*/
bool check_oblique_movement(int i, int j, Movement move){
	if(move == UP_LEFT){
		//there must be the opponent in UP position, no walls between them, a wall behing the opponent and the UP_left move must be possible (no wall)
		if(i>2 && j>0){
			if(game_matrix[i-2][j]!= 0 && game_matrix[i-1][j] == 0 && game_matrix[i-3][j] ==1 && game_matrix[i-2][j-1]==0) return true;
			else return false;
		}
	}
	if(move == UP_RIGHT){
		if(i>2 && j<12){
			if(game_matrix[i-2][j]!= 0 && game_matrix[i-1][j] == 0 && game_matrix[i-3][j] ==1 && game_matrix[i-2][j+1]==0) return true;
			else return false;
		}
	}
	if(move == DOWN_LEFT){
		if(i<10 && j>0){
			if(game_matrix[i+2][j]!= 0 && game_matrix[i+1][j] == 0 && game_matrix[i+3][j] ==1 && game_matrix[i+2][j-1]==0) return true;
			else return false;
		}
	}
	if(move == DOWN_RIGHT){
		if(i<10 && j<12){
			if(game_matrix[i+2][j]!= 0 && game_matrix[i+1][j] == 0 && game_matrix[i+3][j] ==1 && game_matrix[i+2][j+1]==0) return true;
			else return false;
		}
	}
	return false;
}


/**
 * Check if the player with the given status can reach the other side of the map, or if he's trapped.
 * 
 * @param status PlayerStatus of the player for which we're checking the victory.
 * 				 It contains the current position of the player, so I can also identify the player and know where he has to go.
 * 
 * @return true if the player is trapped, false otherwise.
*/
bool is_player_trapped(PlayerStatus status) {
	// Get the real coordinates (convert from 7x7 to 13x13)
	MatrixPoint coordinates = get_index_13matrix_from_7matrix(status.curr_pos.i, status.curr_pos.j);

	// Get the player number (1 or 2) and the goal (GRID_SIZE-1 or 0)
	int player = game_matrix[coordinates.i][coordinates.j];
	int i_goal = (player == 1) ? (GRID_SIZE-1) : 0;	// Player 1 has to reach the bottom, player 2 has to reach the top

	// Reset the "visited" matrix
	int i;
	int j;
	for(i=0; i<GRID_SIZE; i++) {
		for(j=0; j<GRID_SIZE; j++) {
			visited[i][j] = 0;
		}
	}

	// Check if the player can reach the goal
	return !can_reach_goal_from(get_index_7matrix_from_13matrix(coordinates.i, coordinates.j), i_goal);
}

/**
 * Check if the player can reach the given goal from the given position.
 * 
 * @param coordinates The position of the player (in the 7x7 matrix)
 * @param i_goal The i coordinate of the goal (of the 7x7 matrix)
 * 
 * @return true if the player can reach the goal, false otherwise.
*/
bool can_reach_goal_from(MatrixPoint coordinates, int i_goal) {
	// Recursive function, used by is_player_trapped(), using a depth-first search algorithm.

	int i;
	Movement movements[4] = { UP, RIGHT, LEFT, DOWN };
	MatrixPoint coordinates_13x13 = get_index_13matrix_from_7matrix(coordinates.i, coordinates.j);

	// Check if the player is already at the goal
	if (coordinates.i == i_goal)
		return true;

	// Mark the current cell as visited
	visited[coordinates.i][coordinates.j] = 1;

	// Init the possible movements
	if (i_goal != 0) {	// Player 1 has to reach the bottom, go down first and up last
		movements[0] = DOWN;
		movements[3] = UP;
	}

	// Check if the player can reach the goal from the given position
	// I have to check if the player can reach the goal from the 4 adjacent cells
	// If he can, then I can return true, otherwise I have to check if he can reach the goal from the 4 adjacent cells of the adjacent cells, and so on.
	// I can do that recursively, using a depth-first search algorithm, and keeping track of the cells I've already visited.

	for (i=0; i<4; i++) {
		if (check_movement(coordinates_13x13.i, coordinates_13x13.j, movements[i])) {
			// Get the coordinates of the adjacent cell
			MatrixPoint adjacent_coordinates = coordinates;
			if (movements[i] == UP) adjacent_coordinates.i--;
			else if (movements[i] == RIGHT) adjacent_coordinates.j++;
			else if (movements[i] == DOWN) adjacent_coordinates.i++;
			else if (movements[i] == LEFT) adjacent_coordinates.j--;

			// Check if the adjacent cell has already been visited
			if (visited[adjacent_coordinates.i][adjacent_coordinates.j] == 0) {
				// If not, check if the player can reach the goal from the adjacent cell
				if (can_reach_goal_from(adjacent_coordinates, i_goal))
					return true;
			}
		}
	}
	return false;	// Can't reach the goal from the given position withouth passing through a cell that has already been visited
}

/**
 * Check if a wall can be placed in the given position.
 * 
 * @param point The position where the wall has to be placed
 * 
 * @return true if the wall can be placed, false otherwise.
*/
bool is_wall_placement_possible(MatrixPoint point) {
	// Check if the wall is inside the grid
	if (point.i < 0 || point.i >= 12 || point.j < 0 || point.j >= 12)
		return false;

	// Check if there's a wall already in that position (considering that the wall is 2 squares long)
	// If the wall is vertical (i is even, j is odd) then I have to check the given position and the one below
	// If the wall is horizontal (i is odd, j is even) then I have to check the given position and the one on the right
	if (point.i % 2 == 0 && point.j % 2 == 1) {	// Vertical wall
		if (game_matrix[point.i][point.j] != 0 || game_matrix[point.i + 2][point.j] != 0)
			return false;
	}
	else if (point.i % 2 == 1 && point.j % 2 == 0) {	// Horizontal wall
		if (game_matrix[point.i][point.j] != 0 || game_matrix[point.i][point.j + 2] != 0)
			return false;
	}
	else return false;	// Invalid position

	// Check if no walls intersect (a cross).
	// To check that, I have to get the middle point of the wall and check if there's a wall in the other direction.
	// e.g. if I'm placing a vertical wall, I have to check if there's a horizontal wall in the middle of the vertical wall (BOTH above AND below)
	if(point.i % 2 == 0 && point.j % 2 == 1) {	// Vertical wall
		if (game_matrix[point.i + 1][point.j - 1] != 0 && game_matrix[point.i + 1][point.j + 1] != 0)
			return false;
	}
	else if (point.i % 2 == 1 && point.j % 2 == 0) {	// Horizontal wall
		if (game_matrix[point.i - 1][point.j + 1] != 0 && game_matrix[point.i + 1][point.j + 1] != 0)
			return false;
	}
	else return false;	// Invalid position


	// Check if no user is trapped.
	// In order to do that, I can place the wall and check if the player is still able to reach the other side.
	// If he can't, then the wall is not valid, I have to remove it and return false.
	place_wall(point);
	if (is_player_trapped(player1_status) || is_player_trapped(player2_status)) {
		remove_wall(point);
		return false;
	}
	else {
		remove_wall(point);
		return true;
	}
}

/** Rotate the wall that has to be placed by the user */
void rotate_wall(void) {
	LCD_draw_wall(tmp_wall_position.i, tmp_wall_position.j, SCREEN_BACKGROUND_COLOR);
	LCD_redraw_all_walls();

	if(is_wall_horizontal){
		tmp_wall_position.i -=1;
		tmp_wall_position.j +=1;
	}
	else {
		tmp_wall_position.i +=1;
		tmp_wall_position.j -=1;
	}

	LCD_draw_wall(tmp_wall_position.i,tmp_wall_position.j, is_wall_placement_possible(tmp_wall_position) ? POSSIBLE_WALL_POSITION_COLOR : IMPOSSIBLE_WALL_POSITIN_COLOR);
	is_wall_horizontal = !is_wall_horizontal;
}

/**
 * Place a wall in the given position.
 * 
 * @param point The position where the wall has to be placed
*/
void place_wall(MatrixPoint point) {
	if(point.i % 2 == 0 && point.j % 2 == 1) {	// Vertical wall
		game_matrix[point.i][point.j] = 1;
		game_matrix[point.i + 2][point.j] = 1;
	}
	else if (point.i % 2 == 1 && point.j % 2 == 0) {	// Horizontal wall
		game_matrix[point.i][point.j] = 1;
		game_matrix[point.i][point.j + 2] = 1;
	}
	else return;	// Invalid position
}

/**
 * Remove a wall in the given position.
 * 
 * @param point The position where the wall has to be removed
*/
void remove_wall(MatrixPoint point) {
	if(point.i % 2 == 0 && point.j % 2 == 1) {	// Vertical wall
		game_matrix[point.i][point.j] = 0;
		game_matrix[point.i + 2][point.j] = 0;
	}
	else if (point.i % 2 == 1 && point.j % 2 == 0) {	// Horizontal wall
		game_matrix[point.i][point.j] = 0;
		game_matrix[point.i][point.j + 2] = 0;
	}
	else return;	// Invalid position
}


/**
 * Changes the player turn.
 * Restarts the timer and handles everything related.
 * 
 * @param player 1 or 2
*/
void set_player_turn(uint8_t player) {
	if (player == 1) is_player1_turn = true;
	else if (player == 2) is_player1_turn = false;

	is_wall_horizontal = false;
	is_movement_mode = true;
	wanted_movement = NONE;
	turn_time_left = TURN_TIME_SECONDS;
	LCD_scoreboard_update_time(turn_time_left);
	LCD_scoreboard_display_turn(is_player1_turn);

	// Enable for player1, disable for player2
	if (is_player1_turn) {
		if (is_single_board || (!is_single_board && this_board_player_id == PLAYER1)) {
			LCD_draw_enable_movements(player2_status.curr_pos.i, player2_status.curr_pos.j, SCREEN_BACKGROUND_COLOR);
			LCD_draw_enable_movements(player1_status.curr_pos.i, player1_status.curr_pos.j, ENABLE_MOVEMENTS_COLOR);
		}
	}
	// Viceversa
	else {
		if (is_single_board || (!is_single_board && this_board_player_id == PLAYER2)) {
			LCD_draw_enable_movements(player1_status.curr_pos.i, player1_status.curr_pos.j, SCREEN_BACKGROUND_COLOR);
			LCD_draw_enable_movements(player2_status.curr_pos.i, player2_status.curr_pos.j, ENABLE_MOVEMENTS_COLOR);
		}
	}

	if(!is_human && is_single_board && !is_player1_turn) {	// Single board, player vs NPC
		// The NPC is always the player 2
		make_NPC_turn(player2_status);
		
		// Just to test NPC vs NPC; remember also to remove !is_player1_turn from the if statement
		//if(is_player1_turn)
		//	make_NPC_turn(player1_status);
		//else make_NPC_turn(player2_status);
	}
	else if(!is_single_board && !is_human && this_board_player_id == player) {	// Two boards, this board is controlled by NPC
		if(this_board_player_id == PLAYER1)
			make_NPC_turn(player1_status);
		else make_NPC_turn(player2_status);
	}
}


/**
 * "private" function to create a MoveMessage from the given data
*/
void __save_move(bool is_moving_wall, bool is_horizontal, int y, int x) {
	// Create CAN message
	CAN_TxMsg.data[0] = is_player1_turn ? 1 : 2;
	CAN_TxMsg.data[1] = ((is_moving_wall & 0xF) << 4) | (is_horizontal & 0xF);
	CAN_TxMsg.data[2] = y;
	CAN_TxMsg.data[3] = x;

	// Send the message through CAN
	CAN_TxMsg.len = 4;							// 4*8 = 32 (bit, message size).
	CAN_TxMsg.id = is_player1_turn ? 1 : 2;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg(CAN_CONTROLLER_FOR_GAME, &CAN_TxMsg);
}

/**
 * Saves a WALL move (i.e. moving/adding walls)
 * 
 * @param is_horizontal Wheter the wall is horizontal or vertical
 * @param p position of the wall in 13x13 matrix
 * 
*/
void save_wall_move(bool is_horizontal, MatrixPoint p) {
	MatrixPoint p_universal;
	p_universal = get_universal_format_from_13matrix(p.i, p.j);
	__save_move(true, is_horizontal, p_universal.i, p_universal.j);
}

/**
 * Saves a PLAYER move (i.e. moving player)
 * 
 * @param p (i,j) coordinate (relative to 13x13 matrix) of the player
*/
void save_player_move(MatrixPoint p) {
	MatrixPoint p_universal;
	p_universal = get_universal_format_from_13matrix(p.i, p.j);
	__save_move(false, false, p_universal.i, p_universal.j);
}

/**
 * Save a "skipped" move.
 * Used when turn finishes but no move has been done.
*/
void save_skipped_move() {
	__save_move(false, true, -1, -1);
}


/**
 * Decode the received move from CAN (
 * 
 * @param coordinates The position of the player (in the 7x7 matrix)
 * @param i_goal The i coordinate of the goal (of the 7x7 matrix)
 * 
 * @return true if the player can reach the goal, false otherwise.
*/
void decode_received_move_from_CAN(MoveMessage move){
	MatrixPoint p_13;
	uint8_t player_id = move.player_id == 1 ? 2 : 1;
	bool is_moving_wall = move.is_moving_wall;
	bool is_horizontal = move.is_horizontal;

	if(move.x >= 0 && move.y >=0 && move.x <= 6 && move.y <= 6) {	// Turn timer not exceeded	
		if(is_moving_wall)
			p_13 = get_13matrix_from_universal_format(move.y, move.x, true, is_horizontal);
		else
			p_13 = get_13matrix_from_universal_format(move.y, move.x, false, false);

		//draw on the grid the opponent move
		draw_updated_game_matrix(player_id, is_moving_wall, p_13);
		//update game_matrix[13][13]
		update_game_matrix(player_id, is_moving_wall, p_13);
		//update player opponent status
		update_player_status(player_id, is_moving_wall, p_13);
	}

	// Check the victory
	if (has_won(player_id == PLAYER1 ? player1_status : player2_status)) {
		draw_victory_msg(is_player1_turn ? 1 : 2);
		is_game_started = false;
		return;
	}
	
	//change the turn
	if(player_id == 1) set_player_turn(2);
	else set_player_turn(1);
}

/**
 * Update game_matrix[13][13] (place wall o update player position)
 * 
 * @param player_id 
 * @param is_wall true if the player has placed a wall, false if the player has moved
 * @param p position of the wall/player (13x13)
 * 
*/
void update_game_matrix(uint8_t player_id, bool is_wall, MatrixPoint p){
	MatrixPoint ex_player_position;
	if(is_wall)
		//place wall in the matrix
		place_wall(p);
	else {
		//update player position in the matrix
		if(player_id == 1)
			ex_player_position = get_index_13matrix_from_7matrix(player1_status.curr_pos.i, player1_status.curr_pos.j);
		else
			ex_player_position = get_index_13matrix_from_7matrix(player2_status.curr_pos.i, player2_status.curr_pos.j);
		
		game_matrix[ex_player_position.i][ex_player_position.j] = 0;
		game_matrix[p.i][p.j] = player_id;
	}
}

/**
 * Update player status
 * 
 * @param player_id 
 * @param is_wall true if the player has placed a wall, false if the player has moved
 * @param p position of the wall/player (13x13 matrix)
 * 
*/
void update_player_status(uint8_t player_id, bool is_wall, MatrixPoint p_13){
	MatrixPoint p_7;
	if (is_wall) {
		//remove a wall to the player
		if(player_id == 1) player1_status.walls_left--;
		else player2_status.walls_left--;
		
		// Update scoreboard
		LCD_scoreboard_update_walls_left(player_id == PLAYER1, (player_id == PLAYER1) ? player1_status.walls_left : player2_status.walls_left);
	}
	else {
		//update player current position
		p_7 = get_index_7matrix_from_13matrix(p_13.i, p_13.j);
		if (player_id == 1){
			player1_status.curr_pos.i = p_7.i;
			player1_status.curr_pos.j = p_7.j;
		}
		else {
			player2_status.curr_pos.i = p_7.i;
			player2_status.curr_pos.j = p_7.j;
		}
	}
}

/**
* Update game grid: draw the opponent move
 * 
 * @param player_id 
 * @param is_wall true if the player has placed a wall, false if the player has moved
 * @param p position of the wall/player (13x13 matrix)
 * 
 * @return true if the player can reach the goal, false otherwise.
*/
void draw_updated_game_matrix(uint8_t player_id, bool is_wall, MatrixPoint p){
	MatrixPoint ex_index;
	Point2D ex_coordinates, new_coordinates;

	if(is_wall){
		//opponent player has placed a wall; draw the wall in position p
		LCD_draw_wall(p.i, p.j, WALL_COLOR);
	}
	else{
		//opponent has moved
		new_coordinates = get_coordinates_from_indexes(p.i, p.j);

		if(player_id == 1)
			ex_index = get_index_13matrix_from_7matrix(player1_status.curr_pos.i, player1_status.curr_pos.j);
		else
			ex_index = get_index_13matrix_from_7matrix(player2_status.curr_pos.i, player2_status.curr_pos.j);
		
		ex_coordinates = get_coordinates_from_indexes(ex_index.i, ex_index.j);
		
		//clear the actual position
		LCD_draw_pawn(ex_coordinates.x, ex_coordinates.y, SCREEN_BACKGROUND_COLOR);

		//draw in the new position
		if(player_id == 1) LCD_draw_pawn(new_coordinates.x, new_coordinates.y, PLAYER1_COLOR);
		else LCD_draw_pawn(new_coordinates.x, new_coordinates.y, PLAYER2_COLOR);
	}
}

/**
 * Checks if the given queue is empty.
 *
 * @param queue The queue to be checked
 *
 * @return True if the queue is empty, false otherwise
 */
bool isEmpty(Queue *queue) {
    return (queue->front == NULL);
}

/**
 * Adds the given node to the given queue.
 *
 * @param queue The queue to which the node is to be added
 * @param node The node to be added to the queue
 */
void enqueue(Queue *queue, Node *node) {
    // If the queue is empty, the front and rear are the same
    if (isEmpty(queue)) {
        queue->front = node;
        queue->rear = node;
    } else {
        // Add the node to the rear of the queue
        queue->rear->next = node;
        queue->rear = node;
    }
}

/**
 * Removes and returns the first node of the given queue.
 *
 * @param queue The queue from which the node is to be removed
 *
 * @return The first node of the given queue
 */
Node *dequeue(Queue *queue) {
	Node *node;

    // If the queue is empty, return NULL
    if (isEmpty(queue)) {
        return NULL;
    }

    // Remove the first node of the queue
    node = queue->front;
    queue->front = queue->front->next;

    // If the queue is now empty, set the rear to NULL
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    return node;
}


/**
 * Calculate the best movement to make to reach the goal using BFS algorithm.
 *
* 1. Store each cell as a node with their row, column values and distance from source cell.
* 2. Start BFS with source cell.
* 3. Make a visited array with all having false values.
* 4. Keep updating distance from source value in each move.
* 5. Return distance&direction when destination is met, else return -1 (no path exists in between source and destination).
* 
 * @param player The player whose best next step is to be calculated
*/
NextMove calculate_shortest_path(PlayerStatus player) {
	NextMove result;
	MatrixPoint realCoords = get_index_13matrix_from_7matrix(player.curr_pos.i, player.curr_pos.j);
	int8_t playerId = game_matrix[realCoords.i][realCoords.j];
	int goalRow = (playerId == 1) ? (GRID_SIZE - 1) : 0;	// Player 1 has to go down, player 2 has to go up
	int i, j, k, new_i, new_j;
	Queue queue; // Create a queue for BFS
	Node matrix[GRID_SIZE][GRID_SIZE];	// Static matrix of nodes
	Node *node = NULL;
	Node *newNode = NULL;
	
	int iterationsCounter = 0, nestedIterations = 0, maxIterations = GRID_SIZE*GRID_SIZE;

	Movement movements[4] = { UP, RIGHT, LEFT, DOWN };
	if(goalRow != 0) {	// Player 1 has to reach the bottom, go down first and up last
		movements[0] = DOWN;
		movements[3] = UP;
	}

	// Init queue
	queue.front = NULL;
	queue.rear = NULL;

	// Reset the visited matrix (7x7)
	for (i = 0; i < GRID_SIZE; i++) {
		for (j = 0; j < GRID_SIZE; j++) {
			// Mark all cells as not visited
			visited[i][j] = false;

			// Init nodes
			matrix[i][j].bestMovement = NONE;
			matrix[i][j].distance = 0;
			matrix[i][j].i = i;
			matrix[i][j].j = j;
			matrix[i][j].next = NULL;
			matrix[i][j].prev = NULL;
		}
	}

	// Reset the result
	result.length = 0;
	result.bestMovement = NONE;

	// Enqueue the first node
	node = &matrix[player.curr_pos.i][player.curr_pos.j];
	enqueue(&queue, node);
	node = NULL;
	
	// Do a BFS starting from the given cell
	while (!isEmpty(&queue)) {
		iterationsCounter++;
		if(iterationsCounter >= maxIterations)	// Too many iterations, there might be an infinite loop
			break;
		
		// Dequeue a cell from queue and process it
		node = dequeue(&queue);
		
		// Mark the current cell as visited
		visited[node->i][node->j] = true;

		// Get the coordinates of the current cell
		i = node->i;
		j = node->j;

		// Check if the current cell is the goal
		if (i == goalRow) {
			// If it is, return the distance from the source and the best movement
			result.length = node->distance;

			// Get the best movement, by going back from the goal to the node after the source (first step from the origin)
			nestedIterations = 0;
			while (node->prev->prev != NULL) {	// previous node is the start (its previous is NULL)
				nestedIterations++;
				if(nestedIterations >= maxIterations)	// Too many iterations, there might be an infinite loop
					break;
				node = node->prev;
			}
			result.bestMovement = node->bestMovement;
			break;
		}

		// Check all 4 possible movements from the current cell
		for (k = 0; k < 4; k++) {
			realCoords = get_index_13matrix_from_7matrix(i, j);
			
			// Check if the player can move in the given direction
			if (check_movement(realCoords.i, realCoords.j, movements[k])) {
				
				// Get the coordinates of the adjacent cell
				new_i = i;
				new_j = j;
				if (movements[k] == UP) new_i--;
				else if (movements[k] == RIGHT) new_j++;
				else if (movements[k] == DOWN) new_i++;
				else if (movements[k] == LEFT) new_j--;

				// Check if the adjacent cell has already been visited
				if (!visited[new_i][new_j]) {

					// If not, mark it as visited and enqueue it
					visited[new_i][new_j] = true;
					newNode = &matrix[new_i][new_j];
					newNode->distance = node->distance + 1;
					newNode->bestMovement = movements[k];
					newNode->prev = node;
					enqueue(&queue, newNode);
				}
			}
		}
	}

	return result;
}


/**
 * Get the best wall position for the given player.
 *
 * @param opponent The player for which we're calculating the best wall position (the position that slows down the opponent the most)
 *
 * @return The best wall position for the given player. If no position is valid or slows down the opponent, returns (-1, -1).
*/
MatrixPoint get_best_wall_position(PlayerStatus opponent) {
    int best_distance = calculate_shortest_path(opponent).length; // find the longest distance possible for the opponent
    int i, j;
    MatrixPoint best_wall_position, tmp_wall_position;
    best_wall_position.i = -1;
    best_wall_position.j = -1;

    // Check all possible wall positions

    // Horizontal walls
    for (i=0; i<13; i+=2) {
        for (j=1; j<13; j+=2) {
            tmp_wall_position.i = i;
            tmp_wall_position.j = j;
            if (is_wall_placement_possible(tmp_wall_position)) {
                place_wall(tmp_wall_position);
                if (calculate_shortest_path(opponent).length > best_distance) {
                    best_distance = calculate_shortest_path(opponent).length;
                    best_wall_position.i = i;
                    best_wall_position.j = j;
                }
                remove_wall(tmp_wall_position);
            }
        }
    }

    // Vertical walls
    for (i=1; i<13; i+=2) {
        for (j=0; j<13; j+=2) {
            tmp_wall_position.i = i;
            tmp_wall_position.j = j;
            if (is_wall_placement_possible(tmp_wall_position)) {
                place_wall(tmp_wall_position);
                if (calculate_shortest_path(opponent).length > best_distance) {
                    best_distance = calculate_shortest_path(opponent).length;
                    best_wall_position.i = i;
                    best_wall_position.j = j;
                }
                remove_wall(tmp_wall_position);
            }
        }
    }

    return best_wall_position;
}


/**
 * Make the NPC turn (move wall or move player)
 *
 * @param player
 */
void make_NPC_turn(PlayerStatus player) {
	  MatrixPoint realCoords = get_index_13matrix_from_7matrix(player.curr_pos.i, player.curr_pos.j);
	  int8_t player_id = game_matrix[realCoords.i][realCoords.j];
    int8_t opponentId = (player_id == 1) ? 2 : 1;
    int i = 0;
    MatrixPoint best_wall_position;
    NextMove move;
    Movement best_move;
    MatrixPoint new_position;
		
    // Randomly choose if the NPC has to move or place a wall (if he has any left). Probability: 20% to place a wall, 80% to move.
    bool is_moving_wall = (rand() % 100) < 40 && player.walls_left > 0;
  
    Movement movements[4] = { UP, RIGHT, LEFT, DOWN };
    
		if(player_id == 1) {	// Player 1 has to reach the bottom, go down first and up last
        movements[0] = DOWN;
        movements[3] = UP;
    }
		
    // If the NPC has to place a wall, check the wall position that makes the opponent player the most distant from the goal
    if (is_moving_wall) {
        // Get the best wall position
        best_wall_position = get_best_wall_position(opponentId == 1 ? player1_status : player2_status);

        if (best_wall_position.i != -1 && best_wall_position.j != -1) {    // If there's no wall position that makes the opponent player the most distant from the goal, move the player instead (go to the "else" part)

            // Place the wall, by setting the global variables and calling the function to confirm (simulate JOYSTICK SELECT)
            is_movement_mode = false;
            is_wall_horizontal = best_wall_position.i % 2 == 1 && best_wall_position.j % 2 == 0;
            tmp_wall_position.i = best_wall_position.i;
						tmp_wall_position.j = best_wall_position.j;
						LCD_draw_enable_movements(player.curr_pos.i, player.curr_pos.j, SCREEN_BACKGROUND_COLOR);	// Hide available movements
            JOYSEL_Callback();
            return;
        }
    }

    // I can't place a wall, so I have to move the player. If I reach this, it means that the NPC has to move the player, otherwise it would have returned before.

    // Get the best movement
    move = calculate_shortest_path(player);
    best_move = (Movement)move.bestMovement;
    new_position = player.curr_pos;

    // Check if the next movement is NONE (should never happen). If it is, move the player randomly (towards the goal)
    if (best_move == NONE) {
        for(i=0; i<4; i++) {
            // Stop at the first valid movement, which exists because the players can always move
            if (check_movement(new_position.i, new_position.j, movements[i])) {
                best_move = movements[i];
                break;
            }
        }
    }

    // Move the player
    JOY_Handle_Movement_Mode(best_move);
    JOYSEL_Callback();
}


void start_game(){
// init matrix
	init_game_matrix();

	// Init LCD
	LCD_Clear(SCREEN_BACKGROUND_COLOR);
	LCD_draw_game_grid();
	LCD_scoreboard_init();
	is_player1_turn = true;
	LCD_scoreboard_display_turn(true);

	// Set player 1 status
	player1_status.curr_pos.i = 0;
	player1_status.curr_pos.j = 3;
	game_matrix[0][6] = 1;
	player1_status.walls_left = MAX_WALLS; 

	// insert player1 initial position (top centre)
	LCD_insert_gamer(player1_status.curr_pos.i, player1_status.curr_pos.j, true);

	// Set player 2 status
	player2_status.curr_pos.i = 6;
	player2_status.curr_pos.j = 3;
	game_matrix[12][6] = 2;
	player2_status.walls_left = MAX_WALLS; 

	// insert player2 initial position (bottom centre)
	LCD_insert_gamer(player2_status.curr_pos.i, player2_status.curr_pos.j, false);
	
	//set variables
	is_movement_mode = true;
	wanted_movement = NONE;
	is_wall_horizontal = true;
	
	// // Player 1 starts: draw enable movements
	// LCD_draw_enable_movements(0, 3, ENABLE_MOVEMENTS_COLOR);

	// Start turn
	is_game_started = true;			
	enable_timer(TIMER0);
	set_player_turn(1);
}

