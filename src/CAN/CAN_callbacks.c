#include "CAN.h"

#include "../utils/CAN_utils.h"
#include "../utils/game_utils.h"

extern CAN_msg CAN_TxMsg; /* CAN message for sending */
extern CAN_msg CAN_RxMsg; /* CAN message for receiving */



/****************************/
/***** CAN 1 CONTROLLER *****/
/****************************/

void on_CAN1_receive() {
    // Handle handshake received
    if (CAN_RxMsg.id == CAN_HANSHAKE_ID) {

        // The first board receiving the handshake will be player 2, while the sender will be player 1
        if (this_board_player_id == UNSET) {
            this_board_player_id = PLAYER2;
            remote_board_status = AVAILABLE;
            // Update screen
            LCD_game_mode();
        } 

        // Answer with ACK (id = CAN_HANSHAKE_ACK_ID)
        CAN_TxMsg.len = 0;
        CAN_TxMsg.id = CAN_HANSHAKE_ACK_ID;
        CAN_TxMsg.format = STANDARD_FORMAT;
        CAN_TxMsg.type = REMOTE_FRAME;
        CAN_wrMsg(CAN_CONTROLLER_FOR_GAME, &CAN_TxMsg);
    }
    // Handle successful handshake (ack received)
    else if (CAN_RxMsg.id == CAN_HANSHAKE_ACK_ID) {
        // The first board receiving the ack handshake will be player 1, while the receiver will be player 2
        if (this_board_player_id == UNSET) {
            this_board_player_id = PLAYER1;
	        remote_board_status = AVAILABLE;
            // Update screen
            LCD_game_mode();
        }
    }
    // Received other board ready msg
    else if (CAN_RxMsg.id == CAN_BOARD_READY_ID) {
        other_board_ready = true;
        if (curr_board_ready) start_game();
    }
    // Normal player messages
    else {
        MoveMessage msg;

        msg.player_id = CAN_RxMsg.data[0];
        msg.x = CAN_RxMsg.data[3];
        msg.y = CAN_RxMsg.data[2];
        msg.is_moving_wall = (CAN_RxMsg.data[1] >> 4) & 0xF;
        msg.is_horizontal = CAN_RxMsg.data[1] & 0xF;

        decode_received_move_from_CAN(msg);
    }
}

void on_CAN1_send() {

}




/****************************/
/***** CAN 2 CONTROLLER *****/
/****************************/

void on_CAN2_receive() {

}

void on_CAN2_send() {

}
