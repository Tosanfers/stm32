#ifndef __STRAIGHT_LINE__
#define __STRAIGHT_LINE__
#include "stm32f10x.h"

void adjust_speed(int speed, float yaw);
void straight_line(int speed);
void StraightLine_Yaw(int speed, float Yaw);
void StraightLine_Time(int speed, uint32_t time);
void TurnYaw(int yaw,int RightOrLeft);//0左;
void TurnToYaw(int targetyaw);
void TurnToYaw_1(int targetyaw,int RightOrLeft);
#endif