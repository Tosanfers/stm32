#ifndef __MOTOR_H
#define __MOTOR_H 		

#include "stm32f10x.h"

#define	AIN1		PBout(14)
#define	AIN2		PBout(15)
#define	BIN1		PBout(13)
#define	BIN2		PBout(12)

void Motor_Config(void);
void motor(int Motor_1, int Motor_2);
void hold(int time);

#endif







