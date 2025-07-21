#include "stm32f10x.h"

volatile uint32_t currentTick = 0; //用来记录当前时间， 单位是ms

void App_Delay(uint32_t ms);
void app_TIM3_TimeBaseInit(void);
void App_OnboardLed_Init(void);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	app_TIM3_TimeBaseInit();
	App_OnboardLed_Init();
	while(1)
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		App_Delay(100);
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
		App_Delay(100);
	}
}

//中断响应函数
void TIM3_IRQHandler(void)
{
	if (TIM_GetFlagStatus(TIM3,TIM_FLAG_Update) == SET)
	{
		TIM_ClearFlag(TIM3,TIM_FLAG_Update);
		currentTick++;
	}
	
}

void App_Delay(uint32_t ms)
{
	uint32_t expireTime = currentTick + ms;
	while(currentTick < expireTime);
}
void app_TIM3_TimeBaseInit(void)
{
	//开启定时器3的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeStruct;
	TIM_TimeStruct.TIM_Prescaler = 71;
	TIM_TimeStruct.TIM_Period = 999;
	TIM_TimeStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeStruct.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM3,&TIM_TimeStruct);

	//闭合时基单元开关
	TIM_Cmd(TIM3,ENABLE);

	//使能update中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

	//配置NVIC
	NVIC_InitTypeDef NVIC_Strcut;
	NVIC_Strcut.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_Strcut.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Strcut.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Strcut.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_Strcut);
}

void App_OnboardLed_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOC,&GPIO_Struct);
}
