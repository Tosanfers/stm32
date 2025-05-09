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
#include "IMU.h"
#include "straight_line.h"
#include "catch_line.h"
// #include "task.h"
#include "turn.h"
#include "OLED.h"
#include "ENCODER.h"
#include "adc.h"

// 声光提示准备用中断完成
/*------------------------TIPS----------------------------------
 偏航角左减右增
 *----------------------------------------------------------*/

extern IMU_Data imu_data;

volatile float SysYaw = 0;
volatile float cross_yaw_1;
volatile float cross_yaw_2;

void task1(void);
void task2(void);
void task_3_2(void);
void task_4_2(void);
float yaw_left(float yaw_0, float yaw_x);
float yaw_right(float yaw_0, float yaw_x);

int main(void)
{
	Delay_Init();
	IMU_Init(115200); // 延时函数初始化
	Motor_Config();	  // 电机初始化
	// SENSOR_GPIO_Config();	  //数字口初始化
	// Usart3_Init(115200);		//串口初始化
	// IIC接口初始化
	Timer_Init();
	voice_led_init();

	IIC_Init();
	// Encoder_Init_Tim2();
	// Encoder_Init_Tim3();
	OLED_Init();

	//	motor(0,0);
	Delay_ms(500);
	SysYaw = imu_data.yaw;
	// 延时1秒等待系统稳定（可不写）
	// OLED_ShowSignedNum(4, 1, 123, 3);
	// TurnYaw(30,1);

	// TipsOpen();
	// }
	// TurnYaw(10,0);
	// task_3_2();
	task_4_2();
	//  while(1);
	// StraightLine_Time(80, 2000);
	// hold(50);
	// TurnToYaw(-120);
}

void task1(void) // 直走A到B
{
	straight_line(40);
	hold(50);
	TipsOpen();
}

void task2(void)
{
	straight_line(35);
	hold(50);
	TipsOpen();
	CatchToNoLine();
	TipsOpen();
	StraightLine_Yaw(35, orient_over(SysYaw));
	hold(50);
	TipsOpen();
	CatchToNoLine();
	TipsOpen();
	// TurnTime(10, 1, 50);
}

void task_3_2(void) // 对角线策略
{
	if ((SysYaw + 25) >= 180) // 角度右偏28度
	{
		cross_yaw_1 = -(180 - (SysYaw + 25 - 180));
	}
	else
	{
		cross_yaw_1 = SysYaw + 25;
	}
	StraightLine_Yaw(35, cross_yaw_1);
	TipsOpen(); // 到达C点
	int b;
	if ((SysYaw + 10) >= 180) // 角度右偏28度
	{
		b = -(180 - (SysYaw + 10 - 180));
	}
	else
	{
		b = SysYaw + 10;
	}
	TurnToYaw_1(b, 0);
	// straight_line(35);
	// hold(50);
	CatchToNoLine();

	TipsOpen(); // 到达B

	if ((orient_over(SysYaw) - 52) <= -180) // 角度左偏28度
	{
		cross_yaw_2 = (180 + (orient_over(SysYaw) - 52 + 180));
	}
	else
	{
		cross_yaw_2 = orient_over(SysYaw) - 52;
	}
	// TurnToYaw_1(((int)(orient_over(SysYaw))),0);
	TurnYaw(10, 0);
	StraightLine_Yaw(34, cross_yaw_2);
	motor(10, 10);
	Delay_ms(50);
	motor(0, 0);
	TipsOpen(); // 到达D点
	int a;
	if ((orient_over(SysYaw) - 10) <= -180) // 角度左偏28度
	{
		a = (180 + (orient_over(SysYaw) - 10 + 180));
	}
	else
	{
		a = orient_over(SysYaw) - 10;
	}
	TurnToYaw_1(a, 1);
	CatchToNoLine();
	TipsOpen(); // 到达A
	// TurnYaw(21,1);
	TurnToYaw_1(((int)(SysYaw)), 1);
}

void task_4_2()
{
	{
		if ((SysYaw + 25) >= 180) // 角度右偏28度
		{
			cross_yaw_1 = -(180 - (SysYaw + 25 - 180));
		}
		else
		{
			cross_yaw_1 = SysYaw + 25;
		}
		StraightLine_Yaw(35, cross_yaw_1);
		TipsOpen(); // 到达C点
		int b;
		if ((SysYaw + 10) >= 180) // 角度右偏28度
		{
			b = -(180 - (SysYaw + 10 - 180));
		}
		else
		{
			b = SysYaw + 10;
		}
		TurnToYaw_1(b, 0);
		// straight_line(35);
		// hold(50);
		CatchToNoLine();

		TipsOpen(); // 到达B

		if ((orient_over(SysYaw) - 52) <= -180) // 角度左偏28度
		{
			cross_yaw_2 = (180 + (orient_over(SysYaw) - 52 + 180));
		}
		else
		{
			cross_yaw_2 = orient_over(SysYaw) - 52;
		}
		// TurnToYaw_1(((int)(orient_over(SysYaw))),0);
		TurnYaw(10, 0);
		StraightLine_Yaw(34, cross_yaw_2);
		motor(10, 10);
		Delay_ms(50);
		motor(0, 0);
		TipsOpen(); // 到达D点
		int a;
		if ((orient_over(SysYaw) - 10) <= -180) // 角度左偏28度
		{
			a = (180 + (orient_over(SysYaw) - 10 + 180));
		}
		else
		{
			a = orient_over(SysYaw) - 10;
		}
		TurnToYaw_1(a, 1);
		CatchToNoLine();
		TipsOpen(); // 到达A
		// TurnYaw(21,1);
		TurnToYaw_1(((int)(SysYaw)), 1);
	}
	// 1

	{
		if ((SysYaw + 23) >= 180) // 角度右偏28度
		{
			cross_yaw_1 = -(180 - (SysYaw + 23 - 180));
		}
		else
		{
			cross_yaw_1 = SysYaw + 23;
		}
		StraightLine_Yaw(35, cross_yaw_1);
		TipsOpen(); // 到达C点
		int b;
		if ((SysYaw + 10) >= 180) // 角度右偏28度
		{
			b = -(180 - (SysYaw + 10 - 180));
		}
		else
		{
			b = SysYaw + 10;
		}
		TurnToYaw_1(b, 0);
		// straight_line(35);
		// hold(50);
		CatchToNoLine();

		TipsOpen(); // 到达B

		if ((orient_over(SysYaw) - 64) <= -180) // 角度左偏28度
		{
			cross_yaw_2 = (180 + (orient_over(SysYaw) - 64 + 180));
		}
		else
		{
			cross_yaw_2 = orient_over(SysYaw) - 64;
		}
		// TurnToYaw_1(((int)(orient_over(SysYaw))),0);
		TurnYaw(10, 0);
		StraightLine_Yaw(34, cross_yaw_2);
		motor(10, 10);
		Delay_ms(50);
		motor(0, 0);
		TipsOpen(); // 到达D点
		int a;
		if ((orient_over(SysYaw) - 10) <= -180) // 角度左偏28度
		{
			a = (180 + (orient_over(SysYaw) - 10 + 180));
		}
		else
		{
			a = orient_over(SysYaw) - 10;
		}
		TurnToYaw_1(a, 1);
		CatchToNoLine();
		TipsOpen(); // 到达A
		// TurnYaw(21,1);
		TurnToYaw_1(((int)(SysYaw)), 1);
	}
	// 2

	{
		if ((SysYaw + 23) >= 180) // 角度右偏28度
		{
			cross_yaw_1 = -(180 - (SysYaw + 23 - 180));
		}
		else
		{
			cross_yaw_1 = SysYaw + 23;
		}
		StraightLine_Yaw(35, cross_yaw_1);
		TipsOpen(); // 到达C点
		int b;
		if ((SysYaw + 10) >= 180) // 角度右偏28度
		{
			b = -(180 - (SysYaw + 10 - 180));
		}
		else
		{
			b = SysYaw + 10;
		}
		TurnToYaw_1(b, 0);
		// straight_line(35);
		// hold(50);
		CatchToNoLine();

		TipsOpen(); // 到达B

		if ((orient_over(SysYaw) - 70) <= -180) // 角度左偏28度
		{
			cross_yaw_2 = (180 + (orient_over(SysYaw) - 70 + 180));
		}
		else
		{
			cross_yaw_2 = orient_over(SysYaw) - 70;
		}
		// TurnToYaw_1(((int)(orient_over(SysYaw))),0);
		TurnYaw(10, 0);
		StraightLine_Yaw(34, cross_yaw_2);
		motor(10, 10);
		Delay_ms(50);
		motor(0, 0);
		TipsOpen(); // 到达D点
		int a;
		if ((orient_over(SysYaw) - 10) <= -180) // 角度左偏28度
		{
			a = (180 + (orient_over(SysYaw) - 10 + 180));
		}
		else
		{
			a = orient_over(SysYaw) - 10;
		}
		TurnToYaw_1(a, 1);
		CatchToNoLine();
		TipsOpen(); // 到达A
		// TurnYaw(21,1);
		TurnToYaw_1(((int)(SysYaw)), 1);
	}
	// 3
	{
		if ((SysYaw + 24) >= 180) // 角度右偏28度
		{
			cross_yaw_1 = -(180 - (SysYaw + 24 - 180));
		}
		else
		{
			cross_yaw_1 = SysYaw + 24;
		}
		StraightLine_Yaw(35, cross_yaw_1);
		TipsOpen(); // 到达C点
		int b;
		if ((SysYaw + 10) >= 180) // 角度右偏28度
		{
			b = -(180 - (SysYaw + 10 - 180));
		}
		else
		{
			b = SysYaw + 10;
		}
		TurnToYaw_1(b, 0);
		// straight_line(35);
		// hold(50);
		CatchToNoLine();

		TipsOpen(); // 到达B

		if ((orient_over(SysYaw) - 52) <= -180) // 角度左偏28度
		{
			cross_yaw_2 = (180 + (orient_over(SysYaw) - 52 + 180));
		}
		else
		{
			cross_yaw_2 = orient_over(SysYaw) - 52;
		}
		// TurnToYaw_1(((int)(orient_over(SysYaw))),0);
		TurnYaw(10, 0);
		StraightLine_Yaw(34, cross_yaw_2);
		motor(10, 10);
		Delay_ms(50);
		motor(0, 0);
		TipsOpen(); // 到达D点
		int a;
		if ((orient_over(SysYaw) - 10) <= -180) // 角度左偏28度
		{
			a = (180 + (orient_over(SysYaw) - 10 + 180));
		}
		else
		{
			a = orient_over(SysYaw) - 10;
		}
		TurnToYaw_1(a, 1);
		CatchToNoLine();
		TipsOpen(); // 到达A
		// TurnYaw(21,1);
		TurnToYaw_1(((int)(SysYaw)), 1);
	}
	// 4
}

float yaw_left(float yaw_0, float yaw_x)
{
	float yaw_2;
	if ((orient_over(yaw_0) - yaw_x) <= -180) // 角度左偏28度
	{
		yaw_2 = (180 + (orient_over(yaw_0) - yaw_x + 180));
	}
	else
	{
		yaw_2 = orient_over(yaw_0) - yaw_x;
	}
	return yaw_2;
}

float yaw_right(float yaw_0, float yaw_x)
{

	float yaw_1;
	if ((yaw_0 + yaw_x) >= 180) // 角度右偏28度
	{
		yaw_1 = -(180 - (yaw_0 + yaw_x - 180));
	}
	else
	{
		yaw_1 = yaw_0 + yaw_x;
	}
	return yaw_1;
}