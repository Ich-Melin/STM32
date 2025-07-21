#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"

uint32_t blinkInterval = 1000;//闪烁间隔
void APP_OnBoardLed_Init(void);
void APP_Usart1_Init(void);

int main(void)
{
	//优先级分组配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Delay_Init();
	APP_OnBoardLed_Init();
	APP_Usart1_Init();
    My_USART_SendString(USART1,"hello\n");
	while(1)
	{
		//板载led闪烁
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		Delay(blinkInterval);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		Delay(blinkInterval);		
	}
}

//中断响应函数
void USART1_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == SET)
	{
		uint8_t dataRcvd = USART_ReceiveData(USART1);
		switch (dataRcvd)
		{
		case '0':
			blinkInterval = 100;
			break;
		case '1':
			blinkInterval = 200;
			break;
		case '2':
			blinkInterval = 50;
			break;
		default: blinkInterval = 1000;
			break;
		}
	}
}

void APP_OnBoardLed_Init(void)
{
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIO_struct;
	GPIO_struct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_struct.GPIO_Pin = GPIO_Pin_13;
	GPIO_struct.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOC,&GPIO_struct);
}


void APP_Usart1_Init(void)
{
	//引脚初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	//PA9 AF_PP tx
	GPIO_InitTypeDef  GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);

	//pa10 rx 
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_Struct);

	//USART 
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	USART_InitTypeDef USART_Struct;
	USART_Struct.USART_BaudRate	= 115200;
	USART_Struct.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_Struct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Struct.USART_Parity = USART_Parity_No;
	USART_Struct.USART_StopBits = USART_StopBits_1;
	USART_Struct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART1,&USART_Struct);

	//关闭总开关
	USART_Cmd(USART1,ENABLE);

	//配置中断
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

	//配置NVIC
	NVIC_InitTypeDef NVIC_Struct;
	NVIC_Struct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Struct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_Struct);
}

