#include "imu.h"
#include <string.h>
#include "OLED.h"

/* 显示切换 */
volatile IMU_ShowModeSturt IMU_ShowMode;

/* 私有全局变量 */
volatile IMU_Data imu_data;
static uint8_t rx_buffer[20];    // 接收缓冲区
static uint8_t rx_index = 0;     // 接收索引
static uint8_t frame_length = 0; // 当前帧长度
static uint8_t data_ready = 0;   // 数据就绪标志

/* 私有函数声明 */
static void USART1_Init(uint32_t baudrate);
static void ParseFrame(uint8_t *buf);
static uint8_t VerifyChecksum(uint8_t *buf, uint8_t len);

/*----------------------------------------------------------
 * 初始化IMU模块
 * 参数：baudrate - 通信波特率（默认115200）
 *----------------------------------------------------------*/
void IMU_Init(uint32_t baudrate)
{
    USART1_Init(baudrate); // 初始化UART

    // 模块基础配置
    IMU_EnableOutput(IMU_Output_Angles | IMU_Output_SensorRaw); // 默认上报姿态和原始数据
    IMU_ShowMode.Mode = 0;
    IMU_ShowMode.ModeChang = 1;
}

/*----------------------------------------------------------
 * 初始化USART1
 *----------------------------------------------------------*/
static void USART1_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    // 配置TX引脚(PAd:\project_item\STM32\p6_imu.7z9)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 配置RX引脚(PA10)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // USART参数配置
    USART_InitStruct.USART_BaudRate = baudrate;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    // 使能接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // NVIC配置
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART1, ENABLE); // 使能USART
}

/*----------------------------------------------------------
 * USART1中断处理函数
 *----------------------------------------------------------*/
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);

        // 帧头检测：0x55 0x55
        if (rx_index == 0 && data == 0x55)
        {
            rx_buffer[rx_index++] = data;
        }
        else if (rx_index == 1 && data == 0x55)
        {
            rx_buffer[rx_index++] = data;
        }
        else if (rx_index >= 2)
        {
            rx_buffer[rx_index++] = data;

            // 获取帧长度（数据长度+5）
            if (rx_index == 4)
            {
                frame_length = rx_buffer[3] + 5;
            }

            // 完整帧接收
            if (rx_index == frame_length)
            {
                if (VerifyChecksum(rx_buffer, frame_length))
                {
                    ParseFrame(rx_buffer);
                    data_ready = 1; // 设置数据就绪标志
                }
                rx_index = 0; // 重置接收
            }
        }
        else
        {
            rx_index = 0; // 帧错误
        }
    }
}

/*----------------------------------------------------------
 * 显示内容更新
 * 主循环内使用
 *----------------------------------------------------------*/
void IMU_Show(void)
{
    if (IMU_ShowMode.ModeChang)
    {
        OLED_Clear();
        switch (IMU_ShowMode.Mode)
        {
        case 0:
            // 显示静态标签
            OLED_ShowString(1, 1, "Roll:    "); // 预留8字符显示空间
            OLED_ShowString(2, 1, "Pitch:   ");
            OLED_ShowString(3, 1, "Yaw:     ");
            break;
        case 1:
            // 显示静态标签
            OLED_ShowString(1, 1, "Accelx:    "); // 预留8字符显示空间
            OLED_ShowString(2, 1, "Accely:   ");
            OLED_ShowString(3, 1, "Accelz:     ");
            break;
        case 2:
            // 显示静态标签
            OLED_ShowString(1, 1, "Gyrox:    "); // 预留8字符显示空间
            OLED_ShowString(2, 1, "Gyroy:   ");
            OLED_ShowString(3, 1, "Gyroz:     ");
            break;

        default:
            break;
        }
        IMU_ShowMode.ModeChang = 0;
    }

    if (IMU_GetData() != 0)
    {

        switch (IMU_ShowMode.Mode)
        {
        case 0:
            // Roll显示处理（第1行，列7开始）
            int32_t roll_int = (int32_t)imu_data.roll;
            OLED_ShowSignedNum(1, 7, roll_int, 3);

            // Pitch显示处理（第2行）
            int32_t pitch_int = (int32_t)imu_data.pitch;
            OLED_ShowSignedNum(2, 7, pitch_int, 3);

            // Yaw显示处理（第3行）
            int32_t yaw_int = (int32_t)imu_data.yaw;
            OLED_ShowSignedNum(3, 7, yaw_int, 3);
            break;
        case 1:
            int32_t accelx_int = (int32_t)imu_data.accel[0];
            OLED_ShowSignedNum(1, 8, accelx_int, 4);
            int32_t accely_int = (int32_t)imu_data.accel[1];
            OLED_ShowSignedNum(2, 8, accely_int, 4);
            int32_t accelz_int = (int32_t)imu_data.accel[2];
            OLED_ShowSignedNum(3, 8, accelz_int, 4);
            break;
        case 2:
            // 陀螺仪角速度x轴显示（第1行）
            int32_t gyrox_int = (int32_t)imu_data.gyro[0];
            OLED_ShowSignedNum(1, 7, gyrox_int, 4);
            // 陀螺仪角速度y轴显示（第2行）
            int32_t gyroy_int = (int32_t)imu_data.gyro[1];
            OLED_ShowSignedNum(2, 7, gyroy_int, 4);
            // 陀螺仪角速度Z轴显示（第3行）
            int32_t gyroz_int = (int32_t)imu_data.gyro[2];
            OLED_ShowSignedNum(3, 7, gyroz_int, 4);

            break;

        default:
            break;
        }
    }
    // delay_ms(500); // 20Hz刷新率
}

/*----------------------------------------------------------
 * 校验和验证
 * 参数：buf - 数据缓冲区，len - 总长度
 *----------------------------------------------------------*/
static uint8_t VerifyChecksum(uint8_t *buf, uint8_t len)
{
    uint8_t sum = 0;
    for (int i = 0; i < len - 1; i++)
        sum += buf[i];
    return (sum == buf[len - 1]);
}

/*----------------------------------------------------------
 * 解析数据帧
 *----------------------------------------------------------*/
static void ParseFrame(uint8_t *buf)
{
    switch (buf[2])
    {          // 帧ID
    case 0x01: // 姿态角数据
        imu_data.roll = (float)(int16_t)((buf[5] << 8) | buf[4]) / 32768 * 180;
        imu_data.pitch = (float)(int16_t)((buf[7] << 8) | buf[6]) / 32768 * 180;
        imu_data.yaw = (float)(int16_t)((buf[9] << 8) | buf[8]) / 32768 * 180;
        break;

    case 0x03:
    { // 加速度和陀螺仪原始数据
        // 加速度（单位：G）
        int16_t ax = (int16_t)((buf[5] << 8) | buf[4]);
        int16_t ay = (int16_t)((buf[7] << 8) | buf[6]);
        int16_t az = (int16_t)((buf[9] << 8) | buf[8]);
        imu_data.accel[0] = (float)ax / 32768.0f * 4.0f * 9.8f; // 转换为m/s²
        imu_data.accel[1] = (float)ay / 32768.0f * 4.0f * 9.8f;
        imu_data.accel[2] = (float)az / 32768.0f * 4.0f * 9.8f;

        // 陀螺仪（单位：度/秒）
        int16_t gx = (int16_t)((buf[11] << 8) | buf[10]);
        int16_t gy = (int16_t)((buf[13] << 8) | buf[12]);
        int16_t gz = (int16_t)((buf[15] << 8) | buf[14]);
        imu_data.gyro[0] = (float)gx / 32768.0f * 2000.0f;
        imu_data.gyro[1] = (float)gy / 32768.0f * 2000.0f;
        imu_data.gyro[2] = (float)gz / 32768.0f * 2000.0f;
        break;
    }
    }
}

/*----------------------------------------------------------
 * 设置数据上报内容
 * 参数：config - 使用IMU_OutputConfig的位或组合
 *----------------------------------------------------------*/
void IMU_EnableOutput(IMU_OutputConfig config)
{
    uint8_t cmd[] = {0x55, 0xAF, 0x08, 0x01, config, 0x00};
    cmd[5] = 0x55 + 0xAF + 0x08 + 0x01 + config;
    IMU_SendConfig(cmd, sizeof(cmd));
}

/*----------------------------------------------------------
 * 加速度计校准
 *----------------------------------------------------------*/
void IMU_CalibrateAccelerometer(void)
{
    uint8_t cmd[] = {0x55, 0xAF, 0x01, 0x01, 0x00, 0x00};
    cmd[5] = 0x55 + 0xAF + 0x01 + 0x01 + 0x00;
    IMU_SendConfig(cmd, sizeof(cmd));
}

/*----------------------------------------------------------
 * 发送配置命令
 *----------------------------------------------------------*/
void IMU_SendConfig(uint8_t *cmd, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        USART_SendData(USART1, cmd[i]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
            ;
    }
}

/*----------------------------------------------------------
 * 获取最新数据（非阻塞式）
 * 返回：数据指针（NULL表示无新数据）
 *----------------------------------------------------------*/
IMU_Data *IMU_GetData(void)
{
    if (data_ready)
    {
        data_ready = 0;
        return &imu_data;
    }
    return 0;
}


