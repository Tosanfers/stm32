#include "usart.h"
#include "delay.h"
#include "Line.h"
/*********************************************************************
 *  �������ƣ�void Usart3_Init
 *  �������ܣ�����3��ʼ��
 *  ��    �Σ���
 *  ��    ������
 *  ��    ע����
 ********************************************************************/
void Usart3_Init(u32 Baud)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
	
	//USART3_TX		PC.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//USART3_RX		PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;			//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//�Դ���3ͨ��Э����г�ʼ������
	USART_InitStructure.USART_BaudRate = Baud;						//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//����żЧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//˫��ͨ��
	USART_Init(USART3, &USART_InitStructure);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);					//���������ж�	
	USART_Cmd(USART3, ENABLE);

	//�Դ���3�շ��ж�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					//�ж���ѡ�ڶ���
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  		//��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  			//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);
}

u8 USART_RX_STA[22] = { 0 };    //����״̬���	���ֽ�����1+3+16+2  
u8 Num = 0;              //�������ݵĵ�ǰλ��

/*********************************************************************
 *  �������ƣ�Read_Data1(unsigned char addr)
 *  �������ܣ���ȡ�������������ݣ����������������
 *  ��    �Σ�addr����������ַ,*Data��ָ��
 *  ��    ������
 *  ��    ע������1��ʱ����Ҫ�޸�
 *			   	   
 ********************************************************************/    
void Read_Data1(u8 addr, u32 *Data)	     
{	
	u8 y=0;
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, 0x57);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, addr);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
///////////////////////////��������ֵ///////////////////////////////	
	while(1)
  {
		if(Num == 3)
		{
			Num = 0;
			if(USART_RX_STA[0x02] == 0x02)  //�ж�֡β0x02,���߲���ֵ
			{
				*Data = USART_RX_STA[1];			//��������ֵ	       				
				break;
			}        
			break;
		}
    else
    {
      Delay_ms(1);y++;
      if(y==3) { Num = 0;break; }
    }		
	}
///////////////////////////��������ֵ///////////////////////////////	
}
/*********************************************************************
 *  �������ƣ�Read_Data2(unsigned char addr, unsigned int *Data)
 *  �������ܣ���ȡ����ƫ�������ݣ����������������
 *  ��    �Σ�addr����������ַ,*Data��ָ��
 *  ��    ������
 *  ��    ע������1��ʱ����Ҫ�޸�
 *			   	   
 ********************************************************************/    
void Read_Data2(u8 addr, u32 *Data)	     
{	
	unsigned char y=0;
	unsigned int Receive_data = 0;       //���ݻ�����

	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, 0x57);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, addr);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
///////////////////////////ƫ������ֵ///////////////////////////////	
	while(1)
  {		
		if(Num == 5)
		{
			Num = 0;
			if(USART_RX_STA[0x04] == 0x04)  //�ж�֡β0x04,���߲���ֵ
			{
				Receive_data = USART_RX_STA[2];
				Receive_data <<= 8;
				Receive_data |= USART_RX_STA[3];
				
				*Data = USART_RX_STA[1];		//λ�����ݸ�ֵ	  
				*(Data+1) = Receive_data;			//ƫ������ֵ	  
				break;
			}        
			break;
		}
    else
    {
      Delay_ms(1);y++;
      if(y==3) { Num = 0;break; }
    }		
	}
///////////////////////////ƫ������ֵ///////////////////////////////	
}
/*********************************************************************
 *  �������ƣ�Read_Data3(unsigned char addr, unsigned int *Data)
 *  �������ܣ���ȡ����ģ�������ݣ����������������
 *  ��    �Σ�addr����������ַ,*Data��ָ��
 *  ��    ������
 *  ��    ע������1��ʱ����Ҫ�޸�
 *			   	   
 ********************************************************************/    
void Read_Data3(u8 addr, u32 *Data)	     
{	
	unsigned char y=0,i;
	unsigned int Receive_data[ADC_N] = { 0 };       //���ݻ�����,ADC_N��ģ��ֵ

	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, 0x57);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, addr);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
///////////////////////////ģ������ֵ///////////////////////////////	
	while(1)
  {		
		if(Num == 18)
		{
			Num = 0;
			if(USART_RX_STA[0x11] == 0x11)  //�ж�֡β0x11,���߲���ֵ
			{
				for(i=0;i<ADC_N;i++)
				{
					Receive_data[i] = USART_RX_STA[i*2+1];
					Receive_data[i] <<= 8;
					Receive_data[i] |= USART_RX_STA[i*2+2];
					*(Data+i) = Receive_data[i];			//ģ������ֵ	  
				}
				break;
			}        
			break;
		}
    else
    {
      Delay_ms(1);y++;
      if(y==3) { Num = 0;break; }
    }		
	}
///////////////////////////ģ������ֵ///////////////////////////////	
}
/*********************************************************************
 *  �������ƣ�Read_Data4(unsigned char addr, unsigned int *Data)
 *  �������ܣ���ȡ����ȫ�����ݣ����������������
 *  ��    �Σ�addr����������ַ,*Data��ָ��
 *  ��    ������
 *  ��    ע������1��ʱ����Ҫ�޸�
 *			   	   
 ********************************************************************/    
void Read_Data4(u8 addr, u32 *Data)	     
{	
	unsigned char y=0,i;
	unsigned int Receive_data[1+ADC_N] = { 0 };       //���ݻ�����,1��ƫ����+ADC_N��ģ��ֵ

	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, 0x57);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, addr);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
///////////////////////////ȫ����ֵ///////////////////////////////	
	while(1)
  {		
		if(Num == 22)
		{
			Num = 0;
			if(USART_RX_STA[0x15] == 0x15)  //�ж�֡β0x15,���߲���ֵ
			{
				*Data = USART_RX_STA[1];			//��������ֵ	  
				*(Data+1) = USART_RX_STA[2];			//λ�ø�ֵ	  
				for(i=0;i<(1+ADC_N);i++)
				{
					Receive_data[i] = USART_RX_STA[i*2+3];
					Receive_data[i] <<= 8;
					Receive_data[i] |= USART_RX_STA[i*2+4];
					*(Data+2+i) = Receive_data[i];			//1��ƫ����+7��ģ��ֵ��ֵ		  
				}
				break;
			}        
			break;
		}
    else
    {
      Delay_ms(1);y++;
      if(y==5) { Num = 0;break; }
    }		
	}
///////////////////////////ȫ����ֵ///////////////////////////////	
}
/*********************************************************************
 *  �������ƣ�Set_Data
 *  �������ܣ����ù��ܲ���
 *  ��    �Σ�add:�Ĵ�����ַ,Par1: ����1,Par2: ����2
 *  ��    ������
 *  ��    ע����
 ********************************************************************/
void Set_Data1(u8 add,u8 Par1,u8 Par2)	       
{	
///////////////////////////���ù��ܲ���///////////////////////////////	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, 0x4C);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, add);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);

	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, Par1);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, Par2);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
	
//	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//	USART_SendData(USART3, Par3);															//����3,Par3:�������߲ο�ֵ��ʱ��Ż��õ�
//	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);

///////////////////////////���ù��ܲ���///////////////////////////////		
}
/*********************************************************************
 *  �������ƣ�void USART3_IRQHandler
 *  �������ܣ�����3�жϷ�����
 *  ��    �Σ���
 *  ��    ������
 *  ��    ע����
 ********************************************************************/
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
	{
		USART_RX_STA[Num++] =USART_ReceiveData(USART3);	//��ȡ���յ�������
		if(USART_RX_STA[0] != 0x75) { Num = 0; }  //�ж�֡ͷ0x75,�������½���
	} 	
}

