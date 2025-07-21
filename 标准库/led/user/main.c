#include "stm32f10x.h"
#include "delay.h"


int main()
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	Delay_Init();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	
	GPIO_Init(GPIOA,&GPIO_InitStruct);


	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1) == Bit_SET)
			GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
		else	
			GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
		//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
		//Delay(100);
		//GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
		//Delay(100);
	}
}
