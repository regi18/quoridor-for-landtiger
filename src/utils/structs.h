#ifndef __STRUCTS_H 
#define __STRUCTS_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

typedef uint16_t uint16;

//Define the structure of a point in the matrix 
typedef struct MatrixPoint {
	int i;
	int j;
} MatrixPoint;

//Define the structure of a point in the display (pixel position)
typedef struct Point2D {
	uint16_t x;
	uint16_t y;
} Point2D;


typedef struct MoveMessage {
    uint8_t player_id : 8;          		// 1 -> player1, 2-> player2
    uint8_t is_moving_wall : 4;  			// If 0 you are moving the player, otherwise you are placing a wall 
    uint8_t is_horizontal : 4;				// If 0 you are placing a wall in Vertical orientation, otherwise Horizontal orientation. Default to 0 if moving the player.
    int y : 8; 			                // Y coordinate
    int x : 8;    		              // X coordinate
} MoveMessage;


typedef struct PlayerStatus {
    MatrixPoint curr_pos;	// Coordinates between 0 and 6
    int walls_left;
} PlayerStatus;


typedef struct {
    int length; // The length of the shortest path
    int bestMovement;  // The best movement to make to reach the goal
} NextMove;



/**
 * A node in the queue used for BFS.
 */
typedef struct Node {
    int i;  // The row of the node
    int j;  // The column of the node
    int distance;   // The distance of the node from the source
    int bestMovement;  // The best movement to make to reach the goal
    struct Node *next;  // The next node in the queue
    struct Node *prev;  // The previous node in the queue
} Node;

/**
 * A queue used for BFS.
 */
typedef struct Queue {
    Node *front;    // The front of the queue
    Node *rear;     // The rear of the queue
} Queue;

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
#endif
