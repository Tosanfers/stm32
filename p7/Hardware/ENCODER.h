#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f10x.h"

void Encoder_Init_Tim2(void);
void Encoder_Init_Tim3(void);
int Read_Encoder(u8 TIMX);

#endif
