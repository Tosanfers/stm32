#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "Timer.h"

volatile uint8_t TipsOpenOrOff=0; 

void voice_led_init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOB的时钟
    //使用各个外设前必须开启时钟，否则对外设的操作无效

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;					//定义结构体变量

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//GPIO模式，赋值为推挽输出模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;				//GPIO引脚，赋值为第12号引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//GPIO速度，赋值为50MHz

    GPIO_Init(GPIOB, &GPIO_InitStructure);					//将赋值后的构体变量传递给GPIO_Init函数
    //函数内部会自动根据结构体的参数配置相应寄存器
    //实现GPIOB的初始化
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    GPIO_SetBits(GPIOB, GPIO_Pin_1);
}

void voice_led()
{

    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);		//将PB12引脚设置为低电平，蜂鸣器鸣叫,Led亮
    Delay_ms(1000);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    GPIO_SetBits(GPIOB, GPIO_Pin_1);
}


void TipsOpen(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
    TipsOpenOrOff=1;

}

void TipsOff(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
    GPIO_SetBits(GPIOB, GPIO_Pin_1);
    TipsOpenOrOff=0;
}
