#ifndef __GAME_UTILS_H 
#define __GAME_UTILS_H


/* -------------------- Includes -------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "../utils/LCD_utils.h"
#include "CAN_utils.h"
#include "../CAN/CAN.h"

#define GRID_SIZE 7
#define TURN_TIME_SECONDS 20

/* -------------------- Variables & Consts -------------------- */
/*
game_matrix[i][j] meaning:
- i even, j even -> user cell
- i even, j odd  -> vertical wall
- i odd, j even  -> horizontal wall
- i odd, j odd   -> invalid position

Wall cells have 1 if there's a wall, 0 otherwise.
User cells have 1 or 2 if there's a player, 0 otherwise.
Invalid positions have -1 inside (just as a placeholder).
*/
extern int8_t game_matrix[13][13];

typedef enum {
      NONE = -1,
      UP = 0, 
      RIGHT = 1, 
      DOWN = 2, 
      LEFT = 3,
			UP_LEFT = 4,
			UP_RIGHT = 5,
			DOWN_LEFT = 6,
			DOWN_RIGHT = 7
} Movement;

extern PlayerStatus player1_status;
extern PlayerStatus player2_status;

extern bool is_game_started;

extern uint8_t turn_time_left;

extern bool is_player1_turn;	// true -> player1 turn, false -> player2 turn

extern bool is_movement_mode;	// true -> player is moving, false -> player is placing a wall

extern bool is_wall_horizontal;	// true -> horizontal, false -> vertical

extern MatrixPoint tmp_wall_position;	// Coordinates of the wall that the user is placing

extern Movement wanted_movement;	// Place the temporary movement "preference" of the player, to be confirmed when clicking JOY SELECT

extern MatrixPoint wanted_movement_matrix_position; //wanted position indexes in 7x7 matrix

extern bool is_double_movement_mode;

extern bool visited[GRID_SIZE][GRID_SIZE];	// Used to check if a player is trapped using DFS

extern bool is_game_menu;
extern bool is_board_choice;
extern bool is_single_board;	// true -> game is running on a single board; false -> game running via CAN on 2 boards
extern bool is_human;

/* -------------------- Function prototypes -------------------- */
void init_game_matrix(void);
void set_game_matrix(int player, int i, int j);
void remove_player_matrix(int i, int j);
bool has_won(PlayerStatus);
void draw_victory_msg(int player);

MatrixPoint get_index_13matrix_from_7matrix(int i_7, int j_7);
MatrixPoint get_index_7matrix_from_13matrix(int i_13, int j_13);

MatrixPoint get_universal_format_from_13matrix(int i_13, int j_13);
MatrixPoint get_13matrix_from_universal_format(int i, int j, bool is_wall, bool is_horizontal);

bool check_movement(int i, int j, Movement move);
bool check_oblique_movement(int i, int j, Movement move);
bool check_other_player(int i, int j, Movement move);
bool is_player_trapped(PlayerStatus);
bool can_reach_goal_from(MatrixPoint coordinates, int i_goal);

bool is_wall_placement_possible(MatrixPoint);
void rotate_wall(void);
void place_wall(MatrixPoint);
void remove_wall(MatrixPoint);

void set_player_turn(uint8_t player);

void save_wall_move(bool is_horizontal, MatrixPoint p);
void save_player_move(MatrixPoint p);
void save_skipped_move(void);

void decode_received_move_from_CAN(MoveMessage move);

void update_game_matrix(uint8_t player_id, bool is_wall, MatrixPoint p);
void update_player_status(uint8_t player_id, bool is_wall, MatrixPoint p_13);
void draw_updated_game_matrix(uint8_t player_id, bool is_wall, MatrixPoint p);

void start_game(void);

// NPC
NextMove calculate_shortest_path(PlayerStatus player);
void make_NPC_turn(PlayerStatus player);
MatrixPoint get_best_wall_position(PlayerStatus opponent);

// Queue & Node functions
bool isEmpty(Queue* queue);
void enqueue(Queue* queue, Node* node);
Node* dequeue(Queue* queue);

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
#endif

