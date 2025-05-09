#ifndef __IMU_H
#define __IMU_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"

    /* IMU数据结构体 */
    typedef struct
    {
        float roll;     // 横滚角(度)
        float pitch;    // 俯仰角(度)
        float yaw;      // 航向角(度)
        float accel[3]; // 加速度 XYZ (m/s²)
        float gyro[3];  // 角速度 XYZ (度/秒)
    } IMU_Data;

    typedef struct
    {
        uint8_t Mode;
        uint8_t ModeChang;

    } IMU_ShowModeSturt;

    /* 模块配置枚举 */
    typedef enum
    {
        IMU_Output_Angles = 0x01, // 使能姿态角输出
        IMU_Output_Quaternion = 0x02,
        IMU_Output_SensorRaw = 0x04,
        IMU_Output_PortStatus = 0x20
    } IMU_OutputConfig;

    /* 函数声明 */

    void IMU_Show(void);                            // oled显示
    void IMU_Init(uint32_t baudrate);               // 初始化UART和IMU
    void IMU_SendConfig(uint8_t *cmd, uint8_t len); // 发送配置命令
    void IMU_EnableOutput(IMU_OutputConfig config); // 设置数据上报内容
    void IMU_CalibrateAccelerometer(void);          // 加速度计校准
    IMU_Data *IMU_GetData(void);                    // 获取最新数据

#ifdef __cplusplus
}
#endif

#endif /* __IMU_H */
