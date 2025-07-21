#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"

void App_USART1_Init(void);
void App_HCSR04_Init(void);


int main(void)
{
	App_USART1_Init();
	App_HCSR04_Init();
	My_USART_SendString(USART1,"hello\n");
	Delay_Init();
	while(1)
	{
		//向cnt写0
		TIM_SetCounter(TIM1,0);

		//清除cc1和cc2标志位
		TIM_ClearFlag(TIM1,TIM_FLAG_CC1);
		TIM_ClearFlag(TIM1,TIM_FLAG_CC2);

		//开启定时器
		TIM_Cmd(TIM1,ENABLE);

		//向TRIG引脚发送10us的脉冲
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_SET);
		Delay(10);
		GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);

		//等待测量完成
		while(TIM_GetFlagStatus(TIM1,TIM_FLAG_CC1) == RESET);
		while(TIM_GetFlagStatus(TIM1,TIM_FLAG_CC2) == RESET);

		//关闭定时器
		TIM_Cmd(TIM1,DISABLE);

		uint16_t ccr1 = TIM_GetCapture1(TIM1);
		uint16_t ccr2 = TIM_GetCapture2(TIM1);

		float distance = (ccr2 - ccr1) * 1.0e-6f * 340.0f / 2;
		My_USART_Printf(USART1,"distance = %.4f\r\n", distance);
		Delay(100);
	}
}

void App_USART1_Init(void)
{
	//初始化IO引脚
	GPIO_InitTypeDef GPIO_Struct;
	
	//pa9 AF_PP Tx
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);


	//pa10 ipu rx
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_Struct);

	//usart
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	USART_InitTypeDef USART_Struct;
	USART_Struct.USART_BaudRate = 115200;
	USART_Struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Struct.USART_Parity = USART_Parity_No;
	USART_Struct.USART_StopBits = USART_StopBits_1;
	USART_Struct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART1,&USART_Struct);

	//闭合总开关
	USART_Cmd(USART1,ENABLE);
}

void App_HCSR04_Init(void)
{
	//时基单元初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	TIM_TimeBaseInitTypeDef TIM_Struct;
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Struct.TIM_Period = 65535;
	TIM_Struct.TIM_Prescaler = 71;
	TIM_Struct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_Struct);

	//初始化输入捕获
	//初始化IO引脚 PA8 IPD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB,&GPIO_Struct);

	//初始化输入捕获的通道1
	TIM_ICInitTypeDef TIM_ICStruct;
	TIM_ICStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICStruct.TIM_ICFilter = 0;
	TIM_ICStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM1,&TIM_ICStruct);

	//初始化通道2
	TIM_ICStruct.TIM_Channel = TIM_Channel_2;
	TIM_ICStruct.TIM_ICFilter = 0;
	TIM_ICStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICStruct.TIM_ICSelection = TIM_ICSelection_IndirectTI;
	TIM_ICInit(TIM1,&TIM_ICStruct);

	//初始化Trig引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);
}



