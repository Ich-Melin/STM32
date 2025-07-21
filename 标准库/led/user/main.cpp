#include "stm32f10x.h"

extern "C"{
	#include "delay.h"
}

int main()
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	Delay_Init();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;	
	GPIO_Init(GPIOC,&GPIO_InitStruct);

	while(1)
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		Delay(100);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		Delay(100);
	}
}
