#include "stm32f10x.h"
#include "delay.h"

extern void motor(int Motor_1, int Motor_2);
extern uint32_t Get_Systick(void);

void TurnOnLine180(int speed, int LeftOrRight)//left=0,right=1
{
    unsigned int Temp[1] = {0};
    switch (LeftOrRight)
    {
    case 0://左转
        motor(-speed,speed);
        break;
    case 1://右转
        motor(speed,-speed);
        break;
    default:
        motor(-speed, speed);
        break;
    }

    Delay_ms(354);//此为使中间两个传感器离开线上所需时间
    while ((!(Temp[0] == 0x18))) // 0001 1000  //正中间位置
    {
        Read_IICData1(0x01, Temp);
    }

    //惯性修正
    switch (LeftOrRight)
    {
    case 0: // 左转
        motor(-10, 10);
        break;
    case 1: // 右转
        motor(10, -10);
        break;
    default:
        motor(-10, 10);
        break;
    }
    while ((!(Temp[0] == 0x18))) // 0001 1000  //正中间位置
    {
        Read_IICData1(0x01, Temp);
    }
    motor(0, 0);
}

void TurnTime(int speed, int LeftOrRight,uint32_t Time)
{
    uint32_t start_time = Get_Systick();
    switch (LeftOrRight)
    {
    case 0: // 左转
        motor(-speed, speed);
        break;
    case 1: // 右转
        motor(speed, -speed);
        break;
    default:
        motor(-speed, speed);
        break;
    }
    while (Get_Systick()-start_time<Time)
    {
       
    }
    motor(0, 0);
}
