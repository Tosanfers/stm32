#include "stm32f10x.h"
#include "delay.h"
#include "sensor.h"
#include "usart.h"
#include "sys.h"
#include "motor.h"
#include "Line.h"
#include "iic.h"
#include "Timer.h"
#include "Voice_LED.h"
#include "straight_line.h"
#include "catch_line.h"
#include "turn.h"
#include "IMU.h"

// extern IMU_Data imu_data;
// volatile SysYaw;

// void task_init()
// {
//     SysYaw = imu_data.yaw;
// }



void task_3_1() //调头策略
{
    {
        straight_line(50);
        voice_led();
        CatchToNoLine();
        voice_led();
    }
    //到达C点
    TurnOnLine180(30, 1);   //右转向调头
    //调头
    CatchToNoLine();
    voice_led();
    //到达B点
    {
        straight_line(50);
        voice_led();
        //到达A点
        CatchToNoLine();
        voice_led();
        //到达D点
    }//B——>D
    TurnOnLine180(30, 0);   //左转向调头
    //调头
    
    CatchToNoLine();
    voice_led();
    //到达A点
    
}

void task_3_2() //对角线策略
{
    TurnTime(30, 1,300);    //第三个参数为毫秒，t约为0.18T，且应当适当<0.18T，T为转一圈的周期
    straight_line(50);
    voice_led();    //到达C点
    CatchToNoLine();
    voice_led();    //到达B
    TurnTime(30, 0,300);
    straight_line(50);
    voice_led();    //到达D点
    CatchToNoLine();
    voice_led();    //到达A
}

void task_4_1()
{
    for(int i=1;i<=4;i++)
    {
        task_3_1();
        Delay_ms(500);
    }
}

void task_4_2()
{
    for(int i=1;i<=4;i++)
    {
        task_3_2();
        Delay_ms(500);
    }
}