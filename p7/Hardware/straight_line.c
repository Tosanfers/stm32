#include <stdbool.h>
#include <math.h>
#include "IMU.h"
#include "Timer.h"
#include "OLED.h"
#include "delay.h"

// 调节参数（需根据实际调试）
#define KP 1.1f      // 比例系数
#define KI 0.03f     // 积分系数
#define KD 0.3f      // 微分系数
#define DT 0.04f     // 控制周期（单位：秒）
#define MAX_DELTA 50 // 最大差速补偿值
#define V_BASE 50    // 基础速度值

volatile bool is_initialized = false;

static float integral = 0.0f;   // 积分项累加值
static float target_yaw = 0.0f; // 目标航向角
static float prev_error = 0.0f; // 前次误差

extern IMU_Data imu_data;
// 电机控制函数（外部提供）
extern void motor(int Motor_1, int Motor_2);
extern void Read_IICData1(u8 addr, u32 *Data);
extern uint32_t Get_Systick(void);
extern void IMU_Show(void);

// 航向角闭环控制函数
void adjust_speed(int speed, float yaw)
{

    static float target_yaw = 0.0f; // 目标航向角

    static float prev_error = 0.0f; // 前次误差

    /* 初始化 */
    if (!is_initialized)
    {
        target_yaw = yaw; // 锁定初始航向为目标
        is_initialized = true;
        motor(speed, speed); // 初始速度设定
        return;
    }

    /* PID计算 */
    float error = imu_data.yaw - target_yaw;      // 航向偏差（注意符号）
    float derivative = (error - prev_error) / DT; // 微分项

    // 积分项计算（带抗饱和处理）
    float delta_unclamped = KP * error + KI * integral + KD * derivative;
    // if (fabs(delta_unclamped) <= MAX_DELTA)
    // {

    // }
    integral += error * DT; // 仅当输出未饱和时积分

    /* 计算差速补偿 */
    float delta_v = KP * error + KI * integral + KD * derivative;

    // 差速限幅
    delta_v = fminf(fmaxf(delta_v, -MAX_DELTA), MAX_DELTA);

    /* 速度合成 */
    int v_left = (speed - (int)delta_v) * 0.89;
    int v_right = speed + (int)(delta_v);

    // 电机限幅（-100~+100）
    v_left = v_left > 100 ? 100 : (v_left < -100 ? -100 : v_left);
    v_right = v_right > 100 ? 100 : (v_right < -100 ? -100 : v_right);

    /* 执行电机控制 */
    motor(v_left, v_right);
    /* 更新状态 */
    prev_error = error;
}

void straight_line(int speed)
{
    unsigned int Temp[1] = {0};
    // extern float integral;
    integral = 0;
    target_yaw = 0;
    prev_error = 0;
    uint32_t last_time = Get_Systick();
    while (1)
    {

        Read_IICData1(0x01, Temp);
        if ((Temp[0])) // 有线时跳出
        {
            Delay_ms(20);
            Read_IICData1(0x01, Temp);
            if ((Temp[0]))
            {
                break;
            }
        }
        if (Get_Systick() - last_time > 10)
        {
            adjust_speed(speed, imu_data.yaw);
            last_time = Get_Systick();
        }
    }
    motor(0, 0);
    is_initialized = false;
}

void StraightLine_Time(int speed, uint32_t time)
{
    integral = 0;
    target_yaw = 0;
    prev_error = 0;
    uint32_t start_time = Get_Systick();
    while (Get_Systick() - start_time < time)
    {
        adjust_speed(speed, imu_data.yaw);
    }
    motor(0, 0);
    is_initialized = false;
}

void StraightLine_Yaw(int speed, float Yaw) // 沿给定角度的直线行进
{
    unsigned int Temp[1] = {0};
    integral = 0;
    target_yaw = 0;
    prev_error = 0;
    uint32_t last_time = Get_Systick();
    while (1)
    {

        Read_IICData1(0x01, Temp);
        if ((Temp[0]))
        {
            Delay_ms(150);
            Read_IICData1(0x01, Temp);
            if ((Temp[0]))
            {
                break;
            }
        }
        if (Get_Systick() - last_time > 10)
        {
            adjust_speed(speed, Yaw);
            last_time = Get_Systick();
        }
    }
    motor(0, 0);
    is_initialized = false;
}

void TurnYaw(int yaw, int RightOrLeft) // 0左
{
    int currnt_yaw = imu_data.yaw;
    int targetyaw = 0;
    switch (RightOrLeft)
    {
    case 0:
        targetyaw = currnt_yaw - yaw;
        if ((targetyaw) <= -180) // 角度左偏28度
        {
            targetyaw = (180 + (targetyaw + 180));
        }
        else
        {
            targetyaw = targetyaw;
        }

        break;
    case 1:
        targetyaw = currnt_yaw + yaw;
        if ((targetyaw) >= 180) // 角度右偏28度
        {
            targetyaw = -(180 - (targetyaw - 180));
        }
        else
        {
            targetyaw = targetyaw;
        }
        break;

    default:
        break;
    }

    while (abs((int)imu_data.yaw - targetyaw) > 3)
    {
        switch (RightOrLeft)
        {
        case 0:
            motor(-10, 10);
            break;
        case 1:
            motor(10, -10);
            break;

        default:
            break;
        }
    }
    switch ((!RightOrLeft))
    {
    case 0:
        motor(-10, 10);
        break;
    case 1:
        motor(10, -10);
        break;

    default:
        break;
    }
    Delay_ms(20);
    motor(0, 0);
}

void TurnToYaw(int targetyaw)
{

    int currnt_yaw = imu_data.yaw;
    // default:
    //     break;
    // }

    // while (abs((int)imu_data.yaw-targetyaw)>1)
    // {
    //     switch (RightOrLeft)
    //     {
    //     case 0:
    //          motor(-10,10);
    //         break;
    //     case 1:
    //         motor(10,-10);
    //         break;

    //     default:
    //         break;
    //     }
    // }
    while (abs((int)imu_data.yaw - targetyaw) > 2)
    {
        currnt_yaw = imu_data.yaw;
        if (targetyaw > currnt_yaw)
        {
            if ((target_yaw - currnt_yaw) >= 180)
            {
                motor(-10, 10); // 左转
            }
            else
            {
                motor(10, -10); // 右转
            }
            IMU_Show();
        }

        else
        {
            if (abs(currnt_yaw - target_yaw) >= 180)
            {
                motor(10, -10); // 右转
            }
            else
            {
                motor(-10, 10); // 左转
            }
            IMU_Show();
        }
    }

    // switch ((!RightOrLeft))
    // {
    // case 0:
    //      motor(-10,10);
    //     break;
    // case 1:
    //     motor(10,-10);
    //     break;

    // default:
    //     break;
    // }
    // Delay_ms(50);
    motor(0, 0);
}

void TurnToYaw_1(int targetyaw, int RightOrLeft)
{

    int currnt_yaw = imu_data.yaw;
    // switch (RightOrLeft)
    // {
    //     case 0:
    //     targetyaw=currnt_yaw-yaw;
    //     if((targetyaw)<=-180)//角度左偏28度
    // 	{
    // 		targetyaw = (180+(targetyaw+180));
    // 	}
    // 	else
    // 	{
    // 		targetyaw = targetyaw;
    // 	}

    //     break;
    //     case 1:
    //     targetyaw=currnt_yaw+yaw;
    //     if((targetyaw)>=180)//角度右偏28度
    // 	{
    // 		targetyaw = -(180-(targetyaw-180));
    // 	}
    // 	else
    // 	{
    // 		targetyaw = targetyaw;
    // 	}
    //     break;

    // default:
    //     break;
    // }

    while (abs((int)imu_data.yaw - targetyaw) > 2)
    {
        unsigned int Temp[1] = {0};

        switch (RightOrLeft)
        {
        case 0:
            motor(-10, 10);
            break;
        case 1:
            motor(10, -10);
            break;

        default:
            break;
        }
        Read_IICData1(0x01, Temp);
        if ((!Temp[0]))
        {
            // motor(10,10);
            Delay_ms(300);
            // if ((!Temp[0]))
            break;
        }
    }
    // switch ((!RightOrLeft))
    // {
    // case 0:
    //      motor(-10,10);
    //     break;
    // case 1:
    //     motor(10,-10);
    //     break;

    // default:
    //     break;
    // }
    // Delay_ms(50);
    motor(0, 0);
}
