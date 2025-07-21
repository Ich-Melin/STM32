#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"
#include "math.h"

void APP_USART1_Init(void);
void APP_TIM3_Init(void);
void APP_TIM1_Init(void);


int main(void)
{
	APP_TIM1_Init();
	APP_TIM3_Init();
	APP_USART1_Init();
	My_USART_SendString(USART1,"hello\n");
	TIM_SetCompare1(TIM3,200);
	while(1)
	{
		TIM_ClearFlag(TIM1,TIM_FLAG_Trigger);

		while(TIM_GetFlagStatus(TIM1,TIM_FLAG_Trigger) == RESET);

		uint16_t ccr1 = TIM_GetCapture1(TIM1);
		uint16_t ccr2 = TIM_GetCapture2(TIM1);

		float period = ccr1 * 1.0e-6f * 1.0e3f;
		float duty = ((float)ccr2) / ccr1 * 100.0f;

		My_USART_Printf(USART1,"周期=%.3fms, 占空比=%.2f%%\r\n", period,duty);

		Delay(100);
	}
}

void APP_USART1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);

	USART_InitTypeDef USART_Struct;
	USART_Struct.USART_BaudRate = 115200;
	USART_Struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Struct.USART_Mode = USART_Mode_Tx;
	USART_Struct.USART_Parity = USART_Parity_No;
	USART_Struct.USART_StopBits = USART_StopBits_1;
	USART_Struct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART1,&USART_Struct);

	USART_Cmd(USART1,ENABLE);
}

void APP_TIM3_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_Struct;
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Struct.TIM_Period = 999;
	TIM_Struct.TIM_Prescaler = 71;
	TIM_Struct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3,&TIM_Struct);

	TIM_ARRPreloadConfig(TIM3,ENABLE);

	TIM_Cmd(TIM3,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);
	
	TIM_OCInitTypeDef TIM_OCStruct;
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCStruct.TIM_OutputState = ENABLE;
	TIM_OCStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM3,&TIM_OCStruct);
	TIM_CtrlPWMOutputs(TIM3,ENABLE);
	TIM_CCPreloadControl(TIM3,ENABLE);
}

void APP_TIM1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	TIM_TimeBaseInitTypeDef TIM_Strcut;
	TIM_Strcut.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Strcut.TIM_Period = 65535;
	TIM_Strcut.TIM_Prescaler = 71;
	TIM_Strcut.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_Strcut);

	TIM_ARRPreloadConfig(TIM1,ENABLE);

	TIM_Cmd(TIM1,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA,&GPIO_Struct);

	TIM_ICInitTypeDef TIM_ICStruct;
	TIM_ICStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICStruct.TIM_ICFilter = 0;
	TIM_ICStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM1,&TIM_ICStruct);

	TIM_ICStruct.TIM_Channel = TIM_Channel_2;
	TIM_ICStruct.TIM_ICFilter = 0;
	TIM_ICStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICStruct.TIM_ICSelection = TIM_ICSelection_IndirectTI;
	TIM_ICInit(TIM1,&TIM_ICStruct);

	TIM_SelectInputTrigger(TIM1,TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM1,TIM_SlaveMode_Reset);
}
