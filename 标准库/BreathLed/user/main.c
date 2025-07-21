#include "stm32f10x.h"
#include "delay.h"
#include "math.h"

void APP_PWM_Init(void);

int main(void)
{
	APP_PWM_Init();
	Delay_Init();
	while(1)
	{
		float t = GetTick() * 1.0e-3f;
		float duty = 0.5*(sin(2*3.14*t) + 1);
		uint16_t ccr1 = duty * 1000;
		TIM_SetCompare1(TIM1,ccr1);
	}
}

void APP_PWM_Init(void)
{
	//初始化IO引脚 PA8, PB13
	GPIO_InitTypeDef GPIO_Struct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_8;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_Struct);
	
	//配置时基单元 TIM1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_Struct;
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Struct.TIM_Period = 999;
	TIM_Struct.TIM_Prescaler = 71;
	TIM_Struct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_Struct);

	//开启ARR的预加载
	TIM_ARRPreloadConfig(TIM1,ENABLE);

	//闭合开关
	TIM_Cmd(TIM1,ENABLE);

	//初始化输出比较
	//1.初始化输出比较通道1的参数
	TIM_OCInitTypeDef TIM_OCStruct;
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCStruct.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCStruct.TIM_Pulse = 0;

	TIM_OC1Init(TIM1,&TIM_OCStruct);

	//闭合MOE总开关
	TIM_CtrlPWMOutputs(TIM1,ENABLE);

	TIM_CCPreloadControl(TIM1,ENABLE);
}
