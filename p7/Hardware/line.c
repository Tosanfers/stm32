#include "Line.h"
#include <stdbool.h>
#include "motor.h"
#include "sensor.h"
#include "usart.h"
#include "iic.h"
#include "delay.h"
#include "IMU.h"
#include "straight_line.h"

unsigned char lukou_num = 0; // ȫ�ֱ��������⵽·�ڵĴ���
extern IMU_Data imu_data;
// volatile float orient_1;
// volatile float orient_2;	//orient_1����ȡ������Ϊ���صı�׼�Ƕ�

/*************************************
 *�������ƣ�track_zhixian2()
 *�������ܣ�ֱ��ѭ��
 *������
 *˵�������ڻ�ȡ����������,IIC��ȡ����������
 *			�׵׺��ߣ��߿�2���ף������߿����ʵ�������д���ṩ�ĳ���ֻ���ο���
 **************************************/
void track_zhixian2()
{
	unsigned int Temp[1] = {0};				 // ���ݻ�����
	unsigned char num = 0, Temp_data = 0, i; // ������ʱ���

	// Read_Data1(0x01,Temp);
	Read_IICData1(0x01, Temp);

	switch (Temp[0]) // ������߼���1·��С������ߣ�ADC_N·��С�����ұ�
	{				 // 8765 4321 ��·��
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
		break; // 0001 1000  //���м�λ��
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
