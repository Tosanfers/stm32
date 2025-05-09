#include "line.h"
#include "iic.h"
#include "delay.h"




void CatchToNoLine(void)//转弯，出线时停车
{
    unsigned int Temp[1] = {0};              // 数据缓存区
    while (1)
    {
        track_zhixian2();
        Read_IICData1(0x01, Temp);
        if ((!Temp[0]))
        {
            // motor(10,10);
            Delay_ms(100);
            Read_IICData1(0x01, Temp);
            if ((!Temp[0]))
           break;
        }
    }
    motor(0, 0);
}

// void Line_stop(void)    //触线停车
// {
//     unsigned int Temp[1] = {0};              // 数据缓存区
//     while (1)
//     {
//         Read_IICData1(0x01, Temp);
//         if ((!Temp[0]))
//         {
//            break;
//         }
//     }
//     motor(0, 0);
// }