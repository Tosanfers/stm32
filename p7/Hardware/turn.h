#ifndef __TURN__
#define __TURN__

#include "stm32f10x.h"

void TurnOnLine180(int speed, int LeftOrRight);
void TurnTime(int speed, int LeftOrRight, uint32_t Time);

#endif
