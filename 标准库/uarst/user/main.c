#include "stm32f10x.h"
//#include "usart.h"
#include "delay.h"
void usart_init(void);
void My_Usart_Bytesend(USART_TypeDef* Usart, uint8_t* unit, uint16_t size);
uint8_t My_USART_receiveByte(USART_TypeDef *USARTx);

int main(void)
{
	Delay_Init();	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	// PA9 tx
	GPIO_InitTypeDef GPIO_initStrcut;
	GPIO_initStrcut.GPIO_Pin = GPIO_Pin_9;
	GPIO_initStrcut.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_initStrcut.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_initStrcut);

	//PA10 RX;
	GPIO_initStrcut.GPIO_Pin = GPIO_Pin_10;
	GPIO_initStrcut.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_initStrcut);
	
	usart_init();
	//uint8_t bytetosend[] = {1,2,3,4,5};	
	//My_Usart_Bytesend(USART1,bytetosend, sizeof(bytetosend));
	
	
	//亮灯
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_initStrcut.GPIO_Pin = GPIO_Pin_13;
	GPIO_initStrcut.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_initStrcut.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIO_initStrcut);
	while(1)
	{
		uint8_t byterev = My_USART_receiveByte(USART1);
		if(byterev == '0')
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
			//Delay(100);
		else if(byterev == '1')
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);		
			//Delay(100);
	}
}


void usart_init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	USART_InitTypeDef usart;
	usart.USART_BaudRate =  115200;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&usart);
	USART_Cmd(USART1,ENABLE);
}

void My_Usart_Bytesend(USART_TypeDef *usart, uint8_t* unit, uint16_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		while(USART_GetFlagStatus(usart,USART_FLAG_TXE) == RESET);

		USART_SendData(usart,unit[i]);

	}
	
	while(USART_GetFlagStatus(usart,USART_FLAG_TC) == RESET);
}

uint8_t My_USART_receiveByte(USART_TypeDef *USARTx)
{
	while(USART_GetFlagStatus(USARTx,USART_FLAG_RXNE) == RESET);
	return USART_ReceiveData(USARTx);
}
