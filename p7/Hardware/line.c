#include "Line.h"
#include <stdbool.h>
#include "motor.h"
#include "sensor.h"
#include "usart.h"
#include "iic.h"
#include "delay.h"
#include "IMU.h"
#include "straight_line.h"

unsigned char lukou_num = 0; // 全局变量定义检测到路口的次数
extern IMU_Data imu_data;
// volatile float orient_1;
// volatile float orient_2;	//orient_1方向取反，作为返回的标准角度

/*************************************
 *函数名称：track_zhixian2()
 *函数功能：直线循迹
 *参数：
 *说明：串口获取数字量数据,IIC获取数字量数据
 *			白底黑线，线宽2厘米，其他线宽根据实际情况改写，提供的程序只供参考。
 **************************************/
void track_zhixian2()
{
	unsigned int Temp[1] = {0};				 // 数据缓存区
	unsigned char num = 0, Temp_data = 0, i; // 数据临时存放

	// Read_Data1(0x01,Temp);
	Read_IICData1(0x01, Temp);

	switch (Temp[0]) // 这里的逻辑是1路在小车的左边，ADC_N路在小车的右边
	{				 // 8765 4321 （路）
	case 0x01:
		motor(1, 50);
		break; // 0000 0001
	case 0x03:
		motor(10, 50);
		break; // 0000 0011
	case 0x02:
		motor(15, 40);
		break; // 0000 0010
	case 0x06:
		motor(20, 40);
		break; // 0000 0110
	case 0x04:
		motor(25, 40);
		break; // 0000 0100
	case 0x0C:
		motor(35, 40);
		break; // 0000 1100
	case 0x08:
		motor(35, 40);
		break; // 0000 1000
	case 0x18:
		motor(40, 40);
		break; // 0001 1000  //正中间位置
	case 0x10:
		motor(40, 35);
		break; // 0001 0000
	case 0x30:
		motor(40, 35);
		break; // 0011 0000
	case 0x20:
		motor(40, 25);
		break; // 0010 0000
	case 0x60:
		motor(40, 20);
		break; // 0110 0000
	case 0x40:
		motor(40, 15);
		break; // 0100 0000
	case 0xC0:
		motor(50, 10);
		break; // 1100 0000
	case 0x80:
		motor(50, 1);
		break; // 1000 0000

	default:
		motor(20, 20);
		break;
	}
}

float orient_over(float orient)
{
	float orient_2;
	if (orient >= 0)
	{
		orient_2 = orient - 180;
	}
	else
		orient_2 = orient + 180;

	return orient_2;
}
