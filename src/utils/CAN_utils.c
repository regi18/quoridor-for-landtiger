#include "CAN_utils.h"

Player this_board_player_id = UNSET;
RemoteStatus remote_board_status = WAITING;
bool curr_board_ready = false;
bool other_board_ready = false;


void CAN_send_handshake() {
    remote_board_status = WAITING;

	CAN_TxMsg.len = 0;
	CAN_TxMsg.id = CAN_HANSHAKE_ID;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = REMOTE_FRAME;
	CAN_wrMsg(CAN_CONTROLLER_FOR_GAME, &CAN_TxMsg);
}



void CAN_send_current_board_ready() {
	curr_board_ready = true;

	CAN_TxMsg.len = 0;
	CAN_TxMsg.id = CAN_BOARD_READY_ID;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = REMOTE_FRAME;
	CAN_wrMsg(CAN_CONTROLLER_FOR_GAME, &CAN_TxMsg);
}
