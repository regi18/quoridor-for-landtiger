#ifndef __BUTTON_H
#define __BUTTON_H

#include "lpc17xx.h"
#include "../utils/structs.h"
#include "../utils/game_utils.h"
#include "../utils/CAN_utils.h"


void BUTTON_init(void);

// Methods that handle interrupts
void EINT0_IRQHandler(void);
void EINT1_IRQHandler(void);
void EINT2_IRQHandler(void);
void EINT3_IRQHandler(void);

void INT0_Callback(void);
void KEY1_Callback(void);
void KEY2_Callback(void);
void EINT3_Callback(void);



#endif /* __BUTTON_H */
