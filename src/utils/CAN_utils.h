#ifndef __CAN_UTILS_H
#define __CAN_UTILS_H

#include "../CAN/CAN.h"
#include <stdbool.h>


typedef enum {
    WAITING,          // Waiting for second board
    AVAILABLE,        // Second board is available
} RemoteStatus;

typedef enum {
    UNSET,
    PLAYER1,
    PLAYER2
} Player;


extern Player this_board_player_id;
extern RemoteStatus remote_board_status;
extern bool curr_board_ready;
extern bool other_board_ready;

void CAN_send_handshake(void);
void CAN_send_current_board_ready(void);



#endif /* __CAN_UTILS_H */
