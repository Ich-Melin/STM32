#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"

void USART1_Init(void);
void App_ADC1_Init(void);
//void App_OnBoardLED_Init(void);
void App_TIM1_Init(void);

int main(void)
{
	//App_OnBoardLED_Init();
	Delay_Init();
	App_ADC1_Init();
	USART1_Init();
	App_TIM1_Init();
	My_USART_SendString(USART1,"hello\n");
	while(1)
	{
		// #1. 清除EOC标志位
		//ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
		
		// #2. 通过软件启动的方式发送脉冲
		//ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		
		// #3. 等待常规序列转换完成
		//while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		
		// #4. 读取转换的结果
		//uint16_t dr = ADC_GetConversionValue(ADC1);
		
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_JEOC) == RESET);

		uint16_t jdr1 = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);

		ADC_ClearFlag(ADC1,ADC_FLAG_JEOC);

		// #5. 把结果转换成电压
		float voltage = jdr1 * (3.3f / 4095);
		
	/*	if(voltage > 1.5)
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
		}
		else
		{
			GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
		}
*/
		My_USART_Printf(USART1,"%.3f\n",voltage);
		Delay(100);
	}
}

void USART1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_9;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
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

void App_TIM1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	TIM_TimeBaseInitTypeDef TIM_Struct = {0};
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_Struct.TIM_Period = 999;
	TIM_Struct.TIM_Prescaler = 71;
	TIM_Struct.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1,&TIM_Struct);

	TIM_SelectOutputTrigger(TIM1,TIM_TRGOSource_Update);
	TIM_Cmd(TIM1,ENABLE);

}

void App_ADC1_Init(void)
{
	// #1. 初始化PA0引脚，模拟模式
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// #2. 配置ADC模块的时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 六分频
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	// #3. 初始化ADC的基本参数
	
	ADC_InitTypeDef ADC_InitStruct = {0};
	
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE; // 关闭连续模式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right; // 右对齐
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件启动
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent; // 独立模式
	ADC_InitStruct.ADC_NbrOfChannel = 1; // 常规序列1个通道
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	
	ADC_Init(ADC1, &ADC_InitStruct);
	
	// #4. 配置常规序列
	/*ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
	
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);*/
	
	//配置注入序列
	ADC_InjectedSequencerLengthConfig(ADC1,1);
	ADC_ExternalTrigInjectedConvConfig(ADC1,ADC_ExternalTrigInjecConv_T1_TRGO);
	ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
	ADC_InjectedChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_13Cycles5);


	// #5. 闭合ADC的总开关
	ADC_Cmd(ADC1, ENABLE);
}

/*void App_OnBoardLED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}
*/
