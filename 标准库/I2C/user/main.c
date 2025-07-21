#include "stm32f10x.h"

void I2cInit(void);

//I2C发送数据
int My_I2C_sendBytes(I2C_TypeDef* I2CX, uint8_t Addr, uint8_t *Data, uint16_t size);


//接受数据
int My_I2C_ReceiveBytes(I2C_TypeDef* I2CX, uint8_t Addr, uint8_t *pBuffer,  uint16_t size);

uint8_t commands[] = {
	0x00, 0x8d, 0x14, 0xaf, 0xa5
};

int main(void)
{
	I2cInit();
//	RCC_APB2PeriphClockCmd(GPIOB,ENABLE);
	My_I2C_sendBytes(I2C1,0x78,commands,sizeof(commands));

	//初始化系统板led
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIOStruct;
	GPIOStruct.GPIO_Pin = GPIO_Pin_13;
	GPIOStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIOStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIOStruct);
	while(1)
	{
		uint8_t rxvd;
		My_I2C_ReceiveBytes(I2C1,0x78,&rxvd,1);
		if((rxvd & 0x01 << 6) == 0)
		{
			//屏幕点亮 0 输出开漏 0亮
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
		}
		else
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,SET);
		
	}
}

void I2cInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);//开启i2c1时钟
	//施加复位信号
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1,ENABLE);
	//释放复位信号	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1,DISABLE);
	//开启引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef GPIOStruct;
	GPIOStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIOStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIOStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIOStruct);

	I2C_InitTypeDef I2cStruct;
	I2cStruct.I2C_ClockSpeed = 400000;
	I2cStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2cStruct.I2C_Mode = I2C_Mode_I2C;

	I2C_Init(I2C1,&I2cStruct);

	I2C_Cmd(I2C1,ENABLE);
}

// 返回0 成功
// 返回-1 寻址失败
// 返回-2 发送的数据被拒绝
int My_I2C_sendBytes(I2C_TypeDef* I2CX, uint8_t Addr, uint8_t *Data, uint16_t size)
{
	//等待总线空闲
	while(I2C_GetFlagStatus(I2CX,I2C_FLAG_BUSY) == SET);

	//发送起始位
	I2C_GenerateSTART(I2CX,ENABLE);

	while(I2C_GetFlagStatus(I2CX,I2C_FLAG_SB) == RESET);

	//寻址
	I2C_ClearFlag(I2CX,I2C_FLAG_AF);
	I2C_SendData(I2CX,Addr & 0xfe);

	while (1)
	{
		if (I2C_GetFlagStatus(I2CX,I2C_FLAG_ADDR) == SET)
		{
			break;
		}
		if (I2C_GetFlagStatus(I2CX,I2C_FLAG_AF) == SET)
		{
			I2C_GenerateSTOP(I2CX,ENABLE);
			return -1;
		}
	}

	//清楚ADDR；
	I2C_ReadRegister(I2CX,I2C_Register_SR1);
	I2C_ReadRegister(I2CX,I2C_Register_SR2);
	

	//发送数据
	for (uint16_t i = 0; i < size; i++)
	{
		while (1)
		{
			if (I2C_GetFlagStatus(I2CX,I2C_FLAG_AF) == SET)
			{
				I2C_GenerateSTOP(I2CX,ENABLE);
				return -2;//数据接受失败
			}
			if (I2C_GetFlagStatus(I2CX,I2C_FLAG_TXE) == SET)
			{
				break;
			}
		}
		I2C_SendData(I2CX,Data[i]);
	}

	while (1)
	{
		if (I2C_GetFlagStatus(I2CX,I2C_FLAG_AF) == SET)
		{
			I2C_GenerateSTOP(I2CX,ENABLE);
			return -2;
		}
		if (I2C_GetFlagStatus(I2CX,I2C_FLAG_BTF) == SET)
		{
			break;
		}
	}
	
	I2C_GenerateSTOP(I2CX,ENABLE);
	return 0;
}


//接受数据 0 - 发送成功 -1 寻址失败
int My_I2C_ReceiveBytes(I2C_TypeDef *I2CX, uint8_t Addr, uint8_t *pBuffer, uint16_t size)
{
	if (size == 0)
	{
		return 0;
	}

	//等待总线空闲
	while(I2C_GetFlagStatus(I2CX,I2C_FLAG_BUSY) == SET);

	//发送起始位
	I2C_GenerateSTART(I2CX,ENABLE);

	while(I2C_GetFlagStatus(I2CX,I2C_FLAG_SB) == RESET);

	//寻址
	I2C_ClearFlag(I2CX,I2C_FLAG_AF);

	I2C_SendData(I2CX,Addr | 0x01);

	while (1)
	{
		if (I2C_GetFlagStatus(I2CX,I2C_FLAG_ADDR) == SET)
		{
			break;
		}
		if (I2C_GetFlagStatus(I2CX,I2C_FLAG_AF) == SET)
		{
			I2C_GenerateSTOP(I2CX,ENABLE);
			return -1;//寻址失败
		}
	}

	//读取数据
	if (size == 1)
	{
		//写0 NAK;
		I2C_AcknowledgeConfig(I2CX,DISABLE);
		//清楚ADDR	
		I2C_ReadRegister(I2CX,I2C_Register_SR1);
		I2C_ReadRegister(I2CX,I2C_Register_SR2);
		//发送停止位
		I2C_GenerateSTOP(I2CX,ENABLE);

		//等待寄存器接受空
		while(I2C_GetFlagStatus(I2CX,I2C_FLAG_RXNE) == RESET);

		//读取数据
		pBuffer[0] = I2C_ReceiveData(I2CX);
	}
	else
	{
		//向ACK写1
		I2C_AcknowledgeConfig(I2CX,ENABLE);

		//清除ADDR
		I2C_ReadRegister(I2CX,I2C_Register_SR1);
		I2C_ReadRegister(I2CX,I2C_Register_SR2);

		for (uint16_t i = 0; i < size - 1; i++)
		{
			while(I2C_GetFlagStatus(I2CX,I2C_FLAG_RXNE) == RESET);
			//读取数据
			pBuffer[i] == I2C_ReceiveData(I2CX);
		}

		//0写NAK；；
		I2C_AcknowledgeConfig(I2CX,DISABLE);

		//发送停止位
		I2C_GenerateSTOP(I2CX,ENABLE);

		//等待RXNE 读取最后一个数据
		while(I2C_GetFlagStatus(I2CX,I2C_FLAG_RXNE) == RESET);
		pBuffer[size - 1] = I2C_ReceiveData(I2CX);
	}
	return 0;
}
