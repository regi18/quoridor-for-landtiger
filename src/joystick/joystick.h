																			 /*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           joystick.h

**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/
#include "../utils/game_utils.h"
#include "../utils/LCD_utils.h"
#include "../utils/structs.h"
#include "lpc17xx.h"
#include "../utils/CAN_utils.h"


/* lib_joystick */
void joystick_init(void);
void JOYSEL_Callback(void);
void JOYDOWN_Callback(void);
void JOYUP_Callback(void);
void JOYLEFT_Callback(void);
void JOYRIGHT_Callback(void);
void JOY_Handle_Movement_Mode(Movement move);
void JOY_handle_wall_movement(Movement move);

void JOY_UL_Callback(void);
void JOY_UR_Callback(void);
void JOY_DL_Callback(void);
void JOY_DR_Callback(void);
