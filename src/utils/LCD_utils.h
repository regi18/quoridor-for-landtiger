#ifndef __LCD_UTIL_H 
#define __LCD_UTIL_H


/* -------------------- Includes -------------------- */
#include "LPC17xx.h"
#include "../../GLCD/GLCD.h" 
#include "../../GLCD/AsciiLib.h"
#include "../../GLCD/GLCD.h"
#include "structs.h"
#include "game_utils.h"
#include <stdbool.h>
#include <stdio.h>



/* -------------------- Consts -------------------- */
#define SQUARE_LENGTH 27
#define WALL_SPACE 5                // Space between 2 squares. That is also the width of a wall.
#define GRID_PADDING 10
#define MAX_WALLS 8

#define GRID_BORDER_COLOR White
#define WALL_COLOR Black
#define SCREEN_BACKGROUND_COLOR Blue2
#define ENABLE_MOVEMENTS_COLOR Yellow
#define WANTED_MOVEMENT_COLOR Magenta
#define POSSIBLE_WALL_POSITION_COLOR Green
#define IMPOSSIBLE_WALL_POSITIN_COLOR Red
#define PLAYER1_COLOR White
#define PLAYER2_COLOR Red
#define SELECTED_RECT_MENU White


/* -------------------- Consts for main menu -------------------- */
#define MENU_SIDE_PAD 50
#define MENU_RECT_HEIGTH 40
#define MENU_SPACE_BETWEEN 30
#define MENU_RECT_LENGTH 140


/* -------------------- Function prototypes -------------------- */
void LCD_draw_rect(int x, int y, int x_size, int y_size);
void LCD_draw_square(int x0, int y0);
void LCD_draw_wall(int i, int j, uint16_t color);
void LCD_color_square(int x0, int y0, uint16_t color);
void LCD_draw_pawn(int x0, int y0, uint16_t color);
void LCD_init(void);
void LCD_draw_game_grid(void);
void LCD_draw_enable_movements(int i, int j, uint16_t color);
void LCD_insert_gamer(int i, int j, bool is_player_1);
Point2D get_coordinates_from_indexes(int i, int j);
void LCD_scoreboard_write_text(int rect, int x_offset, int y_offset, uint8_t *str);
void LCD_scoreboard_init(void);
void LCD_scoreboard_update_walls_left(bool is_player_1, int walls_left);
void LCD_scoreboard_update_time(int time);
void LCD_scoreboard_display_turn(bool is_player_1_turn);
void LCD_redraw_all_walls(void);
void LCD_draw_message(uint8_t *msg);

void LCD_game_mode(void);
void LCD_player_menu(void);
void LCD_color_rect(int x0, int y0, int height, int length, uint16_t color);

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
#endif
