#include "stm32f10x.h"

void APP_OnBoardLed_Init(void);
void APP_Button_Init(void);

int main(void)
{
	//中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	APP_OnBoardLed_Init();
	APP_Button_Init();
	while(1)
	{
	}
}

void APP_OnBoardLed_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Struct.GPIO_Speed= GPIO_Speed_2MHz;

	GPIO_Init(GPIOC,&GPIO_Struct);
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);

}

void APP_Button_Init(void)
{
	//引脚初始化 5，6
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Struct;
	//pa5
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA,&GPIO_Struct);

	//pa6
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA,&GPIO_Struct);

	//为EXTI5和EXTI6分配引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource6);

	//初始化EXIT线
	EXTI_InitTypeDef EXIT_Struct;
	EXIT_Struct.EXTI_Line = EXTI_Line5;
	EXIT_Struct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXIT_Struct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXIT_Struct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXIT_Struct);

	//pa6
	EXIT_Struct.EXTI_Line = EXTI_Line6;
	EXIT_Struct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXIT_Struct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXIT_Struct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXIT_Struct);

	//配置中断
	NVIC_InitTypeDef NVIC_Struct;
	NVIC_Struct.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Struct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Struct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Struct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_Struct);
}

//中断响应函数
void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetFlagStatus(EXTI_Line5) == SET)
	{
		EXTI_ClearFlag(EXTI_Line5);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
	}
	if (EXTI_GetFlagStatus(EXTI_Line6) == SET)
	{
		EXTI_ClearFlag(EXTI_Line6);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
	}
}
