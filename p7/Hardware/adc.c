#include "adc.h"
#include "delay.h"
#include "usart.h"

/**************************************************************************
函数功能：ACD初始化函数
入口参数：无
返回  值：无
**************************************************************************/

void adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;            //定义结构体GPIO_InitStructure
	ADC_InitTypeDef ADC_InitStructure;          //定义结构ADC_InitStructure

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE);	//开启GPIOA和ADC1时钟

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//不使用扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式，不使用连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件触发，不使用外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目1
	
	ADC_Init(ADC1, &ADC_InitStructure);	//ADC初始化使用adc1
	
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束

	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束

}



/**************************************************************************
函数功能：获得ADC值
入口参数：ch 通道值 0~3
返回  值：返回最近一次ADC1规则组的转换结果
**************************************************************************/

u16  Get_adc(u8 ch)
{   
			//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

/**************************************************************************
函数功能：获取adc的周期数据
入口参数：ch 通道值 0~3
返回  值：times
**************************************************************************/

u16 Get_adc_Average(u8 ch,u8 times)
{ 		    		  			    
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_adc(ch);
		Delay_ms(5);
	}
	return temp_val/times;
	
}

