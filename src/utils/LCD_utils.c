#include "LCD_utils.h"
#include "./CAN_utils.h"
#include "../timers/timer.h"


/**
 * Converts unsigned int to string
*/
#define UI_TO_A_BUF_SIZE 10
unsigned char* ui_to_a(unsigned int i)
{
  /* Room for UINT_DIGITS digits and '\0' */
  static unsigned char buf[UI_TO_A_BUF_SIZE + 1];
  unsigned char *p = buf + UI_TO_A_BUF_SIZE;	/* points to terminating '\0' */
  do {
    *--p = '0' + (i % 10);
    i /= 10;
  } while (i != 0);
  return p;
}


/* Functions ------------------------------------------------------------------*/

/**
 * Draw a rectangle in the given x,y position
 * 
 * @param x x coordinate of the point at the top left of the square 
 * @param y y coordinate of the point at the top left of the square 
 * @param x_size Size of the horizontal side
 * @param y_size Size of the vertical side
*/
void LCD_draw_rect(int x, int y, int x_size, int y_size) {
	LCD_DrawLine(x, y, x + x_size, y, GRID_BORDER_COLOR);
	LCD_DrawLine(x, y, x, y + y_size, GRID_BORDER_COLOR);
	LCD_DrawLine(x, y + y_size, x + x_size, y + y_size, GRID_BORDER_COLOR);
	LCD_DrawLine(x + x_size, y + y_size, x + x_size, y, GRID_BORDER_COLOR);
}


/**
 * Draw a square on the display of the size SQUARE_LENGTH
 * 
 * @param x0 x coordinate of the point at the top left of the square 
 * @param y0 x coordinate of the point at the top left of the square 
*/
void LCD_draw_square(int x0, int y0) {
	LCD_draw_rect(x0, y0, SQUARE_LENGTH, SQUARE_LENGTH);
}


/**
 * color a square on the display 
 * 
 * @param x0 x coordinate of the point at the top left of the square 
 * @param y0 x coordinate of the point at the top left of the square 
*/

void LCD_color_square(int x0, int y0, uint16_t color){
	int k;
	//color square 
	for (k = 1; k < SQUARE_LENGTH; k++)
	{
		LCD_DrawLine(x0+1, y0+k, x0+SQUARE_LENGTH-1, y0+k, color);
	}
}



void LCD_init() {
  	LCD_Initialization();
	LCD_Clear(SCREEN_BACKGROUND_COLOR);
}

/*******************************************************************************
* Function Name  : LCD_draw_game_grid
* Description    : Initialize screen and draw the initial grid
*******************************************************************************/
void LCD_draw_game_grid() {
	int i;
	int j;
	uint16_t x;
	uint16_t y;
	y = GRID_PADDING;

	for (i = 0; i < GRID_SIZE; i++) {
		x = GRID_PADDING;

		for (j = 0; j < GRID_SIZE; j++) {
			LCD_draw_square(x,y);
			x = x + SQUARE_LENGTH + WALL_SPACE;
		}

		y = y + SQUARE_LENGTH + WALL_SPACE;
	}
}



/*******************************************************************************
* Function Name  : get_coordinates_from_indexes
* Description    : get pixel coordinates from matrix indexes of the square (top left)
* Input          : i, j (indexes of the display matrix 13x13)
* Output         : Point2D
* Return         : None
* Attention		 : None
*******************************************************************************/
Point2D get_coordinates_from_indexes(int i, int j){
	uint16_t x;
	uint16_t y;
	Point2D p;
	
	if (i%2 == 0 && j%2 == 0)
	{
		// square position
		
		// j/2 walls and j/2 square
		x = GRID_PADDING + SQUARE_LENGTH * j/2 + WALL_SPACE * j/2;
		// i/2 walls and i/2 square
		y = GRID_PADDING + SQUARE_LENGTH * i/2 + WALL_SPACE * i/2;
	}
	else if (i%2 == 1 && j%2 == 0)
	{
		// horizontal wall position
		
		// j/2 walls and j/2 square
		x = GRID_PADDING + SQUARE_LENGTH * j/2 + WALL_SPACE * j/2;
		// i/2 walls and (i/2+1) square
		y = GRID_PADDING + SQUARE_LENGTH * (i/2 + 1) + WALL_SPACE * i/2;
	}
	else if (i%2 == 0 && j%2 == 1)
	{
		// vertical wall position
		
		// j/2 walls and (j/2+1) square
		x = GRID_PADDING + SQUARE_LENGTH * (j/2 +1) + WALL_SPACE * j/2;
		// i/2 walls and i/2 square
		y = GRID_PADDING + SQUARE_LENGTH * i/2 + WALL_SPACE * i/2;
	}
	else
	{
		//impossible position
	}	
	
	p.x = x;
	p.y = y;
	return p;
}


/*******************************************************************************
* Function Name  : LCD_insert_gamer
* Description    : Inserts a gamer in the grid 
* Input          : i e j (indexes of the matrix 7x7)
*******************************************************************************/
void LCD_insert_gamer(int i, int j, bool is_player_1){
	uint16_t x0;
	uint16_t y0; 
	MatrixPoint mp = get_index_13matrix_from_7matrix (i, j);
	int i_13 = mp.i;
	int j_13 = mp.j;
	Point2D p = get_coordinates_from_indexes(i_13,j_13);
	
	x0 = p.x;
	y0 = p.y;
	
	LCD_draw_pawn(x0, y0, is_player_1 ? PLAYER1_COLOR : PLAYER2_COLOR);
}


/*******************************************************************************
* Function Name  : LCD_draw_wall
* Description    : Draw wall
* Input          : i, j (indexes of the 13x13 matrix)
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_draw_wall(int i, int j, uint16_t color) {
	uint16_t x0, y0, x1, y1;
	int k;
	Point2D p = get_coordinates_from_indexes(i,j);
	//top left coordinates
	x0 = p.x;
	y0 = p.y;
	
	if (i%2 ==0 && j%2==1)
	{
		//vertical wall
		y1 = y0 + 2*SQUARE_LENGTH + WALL_SPACE;
		
		for(k= -1; k<WALL_SPACE-2; k++)
		{
			LCD_DrawLine(x0+k, y0, x0+k, y1, color);
		}
	}
	else if(i%2 ==1 && j%2==0)
	{
		//horizontal wall
		for(k= -1; k<WALL_SPACE-2; k++)
		{
			x1 = x0 + 2*SQUARE_LENGTH + WALL_SPACE;
			LCD_DrawLine(x0, y0+k, x1, y0+k, color);
		}
	}
	else
	{
		//no wall here!
	}
}

/**
 * Draw a pawn on the screen at the given coordinates.
 * 
 * @param x0 x coordinate of the point at the top left of the square
 * @param y0 y coordinate of the point at the top left of the square
 * @param color Color of the pawn
 * 
*/
void LCD_draw_pawn(int x0, int y0, uint16_t color) {
	// This contains 25 rows representing each line of the 25x25 square where the pawn is drawn
	// Each row contains 2 values: the first is the offset from the row start, the second is the length of the line to draw
	char pawn_offset_length[27][2] = { {27, 0}, {27, 0}, {12, 3}, {11, 5}, {10, 7}, {9, 9}, {9, 9}, {9, 9}, {10, 7}, {11, 5}, {8, 11}, {8, 11}, {8, 11}, {10, 7}, {10, 7}, {10, 7}, {10, 7}, {10, 7}, {10, 7}, {9, 9}, {8, 11}, {8, 11}, {6, 15}, {6, 15}, {6, 15}, {6, 15}, {27, 0} };

	
	int i;
	// Draw each line of the pawn
	for (i = 0; i < 25; i++) {
		if(pawn_offset_length[i][1] != 0)	// Skip lines with length 0
			LCD_DrawLine(x0 + pawn_offset_length[i][0], y0 + i, x0 + pawn_offset_length[i][0] + pawn_offset_length[i][1], y0 + i, color);
	}
}

/**
 * Redraw all the walls on the screen.
 * This is used when a wall is moved on the screen.
*/
void LCD_redraw_all_walls() {
	int i;
	int j;
	
	// Redraw horizontal walls, which have odd i and even j
	for (i = 1; i < 13; i += 2) {
		for (j = 0; j < 13; j += 2) {
			// Check if there is a wall; if so, draw it and skip the next position (because the wall is 2 positions long)
			if (game_matrix[i][j] != 0) {
				LCD_draw_wall(i, j, WALL_COLOR);
				j += 2;	// Skip the next position
			}
		}
	}

	// Redraw vertical walls, which have even i and odd j, by iterating the matrix by columns
	for (j = 1; j < 13; j += 2) {
		for (i = 0; i < 13; i += 2) {
			// Check if there is a wall; if so, draw it and skip the next position (because the wall is 2 positions long)
			if (game_matrix[i][j] != 0) {
				LCD_draw_wall(i, j, WALL_COLOR);
				i += 2;	// Skip the next position
			}
		}
	}
}

/*******************************************************************************
* Function Name  : LCD_draw_message
* Description    : Draws the given message between the grid and the scoreboard
* Input          : msg
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void LCD_draw_message(uint8_t *msg){
		//First cancel whatever was written before to avoid mixing messages
		int i = 0;
		uint16_t y0 = GRID_SIZE * SQUARE_LENGTH + (GRID_SIZE - 1) * WALL_SPACE + GRID_PADDING + 2;  // starting Y-coordinate, to increment
		
		for(i = 0; i < 15; i++){
			LCD_DrawLine(0, y0, 240, y0, SCREEN_BACKGROUND_COLOR);
			y0++;
		}
		
		//Then write the message
		GUI_Text(GRID_PADDING, GRID_SIZE * SQUARE_LENGTH + (GRID_SIZE - 1) * WALL_SPACE + GRID_PADDING + 2, msg, White, SCREEN_BACKGROUND_COLOR);
		init_timer(TIMER1, 1500, MCR_INTERRUPT_RESET_AND_STOP_ON_MR0);
		enable_timer(TIMER1);
}


/*****************************************************************************************/
/*                                                                                       */
/*		 							 SCOREBOARD 									     */
/*                                                                                       */
/*****************************************************************************************/

/**
 * Draw the scoreboard at the bottom of the screen.
 * Initialize it with default values.
*/
void LCD_scoreboard_init() {
	const int padding = WALL_SPACE;
	// Starting x,y position to draw
	const int x = GRID_PADDING;
	const int y = GRID_SIZE * SQUARE_LENGTH + (GRID_SIZE - 1) * WALL_SPACE + GRID_PADDING * 3;
	// Size of the rectangles of the scoreboard
	const int RECT_X_LENGTH = (MAX_X - GRID_PADDING * 2 - (2 * WALL_SPACE)) / 3;
	const int RECT_Y_LENGTH = 50;

	// Init x coord for the other rects
	const int rect1_x0 = x + RECT_X_LENGTH + padding;
	const int rect2_x0 = rect1_x0 + RECT_X_LENGTH + padding;

	LCD_draw_rect(x, y, RECT_X_LENGTH, RECT_Y_LENGTH);
	LCD_draw_rect(rect1_x0, y, RECT_X_LENGTH, RECT_Y_LENGTH);
	LCD_draw_rect(rect2_x0, y, RECT_X_LENGTH, RECT_Y_LENGTH);

	LCD_scoreboard_write_text(0, 0, 0, (uint8_t *)"P1 Walls");
	LCD_scoreboard_write_text(2, 0, 0, (uint8_t *)"P2 Walls");

	// Show initial values
	LCD_scoreboard_update_walls_left(false, MAX_WALLS);
	LCD_scoreboard_update_walls_left(true, MAX_WALLS);
	LCD_scoreboard_update_time(20);
}


/**
 * Write some text in the rectangles of the scoreboard.
 * 
 * @param rect 0,1,2 the index of the wanted rectangle
 * @param x_offset x coordinates from the upper left corner of the selected rectangle
 * @param y_offset y coordinates from the upper left corner of the selected rectangle
*/
void LCD_scoreboard_write_text(int rect, int x_offset, int y_offset, uint8_t *str) {
	const int RECT_X_LENGTH = (MAX_X - GRID_PADDING * 2 - (2 * WALL_SPACE)) / 3;
	const int y = GRID_SIZE * SQUARE_LENGTH + (GRID_SIZE - 1) * WALL_SPACE + GRID_PADDING * 3 + WALL_SPACE;
	int x = GRID_PADDING + WALL_SPACE;

	if (rect == 1) x += RECT_X_LENGTH + WALL_SPACE;
	else if (rect == 2) x += (RECT_X_LENGTH + WALL_SPACE) * 2;

	GUI_Text(x + x_offset, y + y_offset, str, GRID_BORDER_COLOR, SCREEN_BACKGROUND_COLOR);
}

/**
 * Update the number of walls left on the scoreboard
 * 
 * @param is_player_1 Wheter chaning for player1 or player2
 * @param walls_left number of walls that the player can still use
*/
void LCD_scoreboard_display_turn(bool is_player_1_turn) {
	char str[15];
	sprintf(str, "PLAYER %s TURN", is_player_1_turn ? "1" : "2");
	GUI_Text(65, MAX_Y - 17, (uint8_t*)str, is_player_1_turn ? PLAYER1_COLOR : PLAYER2_COLOR, SCREEN_BACKGROUND_COLOR);
}

/**
 * Update the number of walls left on the scoreboard
 * 
 * @param is_player_1 Wheter chaning for player1 or player2
 * @param walls_left number of walls that the player can still use
*/
void LCD_scoreboard_update_walls_left(bool is_player_1, int walls_left) {
	// Convert to char the number and write to screen
	LCD_scoreboard_write_text(is_player_1 ? 0 : 2, 25, 22, ui_to_a(walls_left));
}


/**
 * Update the time left on the scoreboard
 * 
 * @param p1_walls_left number of walls that the player1 can still use
 * @param p2_walls_left number of walls that the player2 can still use
 * @param time_left 
*/
void LCD_scoreboard_update_time(int time) {
	// Convert to char the number, append "s" and write to screen
	char str[10];
	sprintf(str, "%02d%s", time, "s");
	LCD_scoreboard_write_text(1, 20, 12, (uint8_t *) str);
}





/*****************************************************************************************/
/*                                                                                       */
/*		 							 MOVEMENTS	 									     */
/*                                                                                       */
/*****************************************************************************************/


/*******************************************************************************
* Function Name  : LCD_draw_enable_movements
* Description    : Draw possible movements with yellow squares
* Input          : i, j (indexes of the display matrix 7x7)
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/

void LCD_draw_enable_movements(int i, int j, uint16_t color){	
	//get the 13x13 matrix indexes
	MatrixPoint mp = get_index_13matrix_from_7matrix(i,j);
	int i_13 = mp.i;
	int j_13 = mp.j;
	Point2D p;
	
	//check the move UP
	if(check_movement(i_13, j_13, UP)){
		//move UP is possible
		
		//Check that the other player isn't already there
		if(check_other_player(i_13, j_13, UP)){
			//check if the player can jump the other player
			if(check_movement(i_13 -2, j_13, UP)){
				//jump the other player
				//draw the up-up square
				p = get_coordinates_from_indexes(i_13 - 4, j_13);
				LCD_color_square(p.x, p.y, color);
			}
		}
		else{
			//there isn't the other player
			//draw the up square
			p = get_coordinates_from_indexes(i_13 -2, j_13);
			LCD_color_square(p.x, p.y, color);
		}
	}

	//check the move LEFT
	if(check_movement(i_13, j_13, LEFT)){
		//move LEFT is possible
		
		//Check that the other player isn't already there
		if(check_other_player(i_13, j_13, LEFT)){
			//the other player is already there
			//check if the player can jump the other player
			if(check_movement(i_13, j_13-2, LEFT)){
				//jump the other player
				//draw the left-left square
				p = get_coordinates_from_indexes(i_13, j_13-4);
				LCD_color_square(p.x, p.y, color);
			}
		}
		else{
			//there isn't the other player
			//draw the left square
			p = get_coordinates_from_indexes(i_13, j_13-2);
			LCD_color_square(p.x, p.y, color);
		}
	}
	
	if(check_movement(i_13, j_13, DOWN)){
		//move DOWN is possible
		
		//Check that the other player isn't already there
		if(check_other_player(i_13, j_13, DOWN)){
			//the other player is already there
			//check if the player can jump the other player
			if(check_movement(i_13 +2 , j_13, DOWN)){
				//jump the other player
				//draw the down-down square
				p = get_coordinates_from_indexes(i_13 + 4, j_13);
				LCD_color_square(p.x, p.y, color);
			}
		}
		else{
			//there isn't the other player
			//draw the down square
			p = get_coordinates_from_indexes(i_13+2, j_13);
			LCD_color_square(p.x, p.y, color);
		}
	}
	
	//check the move RIGHT
	if(check_movement(i_13, j_13, RIGHT)){
		//move RIGHT is possible
		
		//Check that the other player isn't already there
		if(check_other_player(i_13, j_13, RIGHT)){
			//the other player is already there
			//check if the player can jump the other player
			if(check_movement(i_13, j_13+2, RIGHT)){
				//jump the other player
				//draw the right-right square
				p = get_coordinates_from_indexes(i_13, j_13+4);
				LCD_color_square(p.x, p.y, color);
			}
		}
		else{
			//there isn't the other player
			//draw the right square
			p = get_coordinates_from_indexes(i_13, j_13+2);
			LCD_color_square(p.x, p.y, color);
		}
	}
	
	//check the move UP_LEFT
	if(check_oblique_movement(i_13, j_13, UP_LEFT)){
		//move UP_LEFT is possible
		p = get_coordinates_from_indexes(i_13-2, j_13-2);
		LCD_color_square(p.x, p.y, color);
	}
	
	//check the move UP_RIGHT
	if(check_oblique_movement(i_13, j_13, UP_RIGHT)){
		//move UP_RIGHT is possible
		p = get_coordinates_from_indexes(i_13-2, j_13+2);
		LCD_color_square(p.x, p.y, color);
	}
	
	//check the move DOWN_LEFT
	if(check_oblique_movement(i_13, j_13, DOWN_LEFT)){
		//move DOWN_LEFT is possible
		p = get_coordinates_from_indexes(i_13+2, j_13-2);
		LCD_color_square(p.x, p.y, color);
	}
	
	//check the move DOWN_RIGHT
	if(check_oblique_movement(i_13, j_13, DOWN_RIGHT)){
		//move DOWN_RIGHT is possible
		p = get_coordinates_from_indexes(i_13+2, j_13+2);
		LCD_color_square(p.x, p.y, color);
	}
	
}

/**
 * Draw the GUI game mode 
 * 
 * @param 
*/

void LCD_game_mode(){
	int start_text = 77;
	int rect_text = 70;
	char str[3];
	is_board_choice = true;
	is_game_menu = true;
	
	LCD_Clear(SCREEN_BACKGROUND_COLOR);
	
	GUI_Text(start_text, MENU_SIDE_PAD - 10, (uint8_t*) "Select the", GRID_BORDER_COLOR, SCREEN_BACKGROUND_COLOR);
	GUI_Text(start_text + 4, MENU_SIDE_PAD + 10, (uint8_t*) "GAME MODE", GRID_BORDER_COLOR, SCREEN_BACKGROUND_COLOR);
	

	LCD_draw_rect(MENU_SIDE_PAD, (MAX_Y/2)- MENU_RECT_HEIGTH, MENU_RECT_LENGTH, MENU_RECT_HEIGTH) ;
	LCD_color_rect(MENU_SIDE_PAD,(MAX_Y/2)- MENU_RECT_HEIGTH, MENU_RECT_HEIGTH, MENU_RECT_LENGTH, is_single_board ? SELECTED_RECT_MENU : SCREEN_BACKGROUND_COLOR);
	GUI_Text(rect_text, (MAX_Y/2)- 27 , (uint8_t*) "Single Board", is_single_board ? SCREEN_BACKGROUND_COLOR : SELECTED_RECT_MENU, is_single_board ? SELECTED_RECT_MENU : SCREEN_BACKGROUND_COLOR );

	LCD_draw_rect(MENU_SIDE_PAD, (MAX_Y/2)+ MENU_SPACE_BETWEEN, MENU_RECT_LENGTH, MENU_RECT_HEIGTH);
	LCD_color_rect(MENU_SIDE_PAD,(MAX_Y/2)+ MENU_SPACE_BETWEEN, MENU_RECT_HEIGTH, MENU_RECT_LENGTH, !is_single_board ? SELECTED_RECT_MENU : SCREEN_BACKGROUND_COLOR);

	if (remote_board_status == WAITING) {
		GUI_Text(rect_text - 8, (MAX_Y/2)+ MENU_SPACE_BETWEEN + 13, (uint8_t*) "No second Board", White, Red);
	} 
	else if(remote_board_status == AVAILABLE) {
		GUI_Text(rect_text +7, (MAX_Y/2)+ MENU_SPACE_BETWEEN + 13, (uint8_t*) "Two Boards", !is_single_board ? SCREEN_BACKGROUND_COLOR : SELECTED_RECT_MENU, !is_single_board ? SELECTED_RECT_MENU : SCREEN_BACKGROUND_COLOR);

		sprintf(str, "PLAYER %s", this_board_player_id == PLAYER1 ? "1" : "2");
		GUI_Text(rect_text + 16, MAX_Y - MENU_SPACE_BETWEEN, (uint8_t*)str,  this_board_player_id == PLAYER1 ? PLAYER1_COLOR : PLAYER2_COLOR, SCREEN_BACKGROUND_COLOR);
	}
	
}

void LCD_player_menu(){

	LCD_Clear(SCREEN_BACKGROUND_COLOR);
	//show the appropriate message based on what the user selected
	if(is_single_board){
		GUI_Text(39, MENU_SIDE_PAD - 10, (uint8_t*) "Single Board: select", White, SCREEN_BACKGROUND_COLOR);
		GUI_Text(39, MENU_SIDE_PAD + 10, (uint8_t*) "the opposite player", White, SCREEN_BACKGROUND_COLOR);
	} else {
		GUI_Text(44, MENU_SIDE_PAD - 10, (uint8_t*) "Two Boards: select", White, SCREEN_BACKGROUND_COLOR);
		GUI_Text(73, MENU_SIDE_PAD + 10, (uint8_t*) "your player", White, SCREEN_BACKGROUND_COLOR);
	}
	
	LCD_draw_rect(MENU_SIDE_PAD, (MAX_Y/2)- MENU_RECT_HEIGTH, MENU_RECT_LENGTH, MENU_RECT_HEIGTH) ;
	LCD_color_rect(MENU_SIDE_PAD,(MAX_Y/2)- MENU_RECT_HEIGTH, MENU_RECT_HEIGTH, MENU_RECT_LENGTH, is_human ? SELECTED_RECT_MENU : SCREEN_BACKGROUND_COLOR);
	GUI_Text(99, (MAX_Y/2)- 27 , (uint8_t*) "Human", is_human ? SCREEN_BACKGROUND_COLOR : SELECTED_RECT_MENU, is_human ? SELECTED_RECT_MENU : SCREEN_BACKGROUND_COLOR);	
	
	LCD_draw_rect(MENU_SIDE_PAD, (MAX_Y/2)+ MENU_SPACE_BETWEEN, MENU_RECT_LENGTH, MENU_RECT_HEIGTH) ;
	LCD_color_rect(MENU_SIDE_PAD,(MAX_Y/2)+ MENU_SPACE_BETWEEN, MENU_RECT_HEIGTH, MENU_RECT_LENGTH, !is_human ? SELECTED_RECT_MENU : SCREEN_BACKGROUND_COLOR);
	GUI_Text(107, (MAX_Y/2)+ MENU_SPACE_BETWEEN + 13 , (uint8_t*) "NPC", !is_human ? SCREEN_BACKGROUND_COLOR : SELECTED_RECT_MENU, !is_human ? SELECTED_RECT_MENU : SCREEN_BACKGROUND_COLOR);	

}

void LCD_color_rect(int x0, int y0, int height, int length, uint16_t color){
	int i;
	
	for(i=1; i<height; i++){
		LCD_DrawLine(x0+1, y0 + i, x0+length-1, y0+i, color);
	}

}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
