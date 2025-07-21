#include "stm32f10x.h"
#include "delay.h"
#include "button.h"

Button_TypeDef button;

void APP_SPI1_Init(void);
void APP_SPI_MasterTransmitReceive(SPI_TypeDef* SPIX, const uint8_t *pdataTX,  uint8_t *pdataRx, uint16_t size);
void APP_W25Q64_SaveByte(uint8_t Byte);
uint8_t APP_W25Q64_LoadByte(void);
void APP_OnBoardLed_Init(void);
void APP_button_Init(void);
void button_clicked_cb(uint8_t clicks);


int main(void)
{
	/*RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_Struct);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIO_Struct);

	uint8_t current = Bit_SET, previous = Bit_SET;
	
	while(1)
	{
		previous = current;
		
		current = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
		
		if(current != previous)
		{
			if(current == Bit_SET)
			{
				// 改变LED
				if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET)
				{
					GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
				}
				else
				{
					GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
				}
			}
			else
			{
			}
			Delay(10);
		}
	}*/
	
	APP_SPI1_Init();
	APP_OnBoardLed_Init();
	APP_button_Init();

	uint8_t byte = APP_W25Q64_LoadByte();

	if (byte == 0)
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
	}
	else
	{
		GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
	}
	
}

void button_clicked_cb(uint8_t clicks)
{
	if(clicks == 1)
	{
		if (GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13) == Bit_SET)	
		{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
			APP_W25Q64_SaveByte(0x01);
		}
		else{
			GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_SET);
			APP_W25Q64_SaveByte(0x00);
		}
		
	}
}
void APP_OnBoardLed_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOC,&GPIO_Struct);
	GPIO_WriteBit(GPIOC,GPIO_Pin_13,Bit_RESET);
}
void APP_button_Init(void)
{
	Button_InitTypeDef Button_Struct;
	Button_Struct.GPIOx = GPIOA;
	Button_Struct.GPIO_Pin = GPIO_Pin_0;
	Button_Struct.button_clicked_cb = button_clicked_cb;

	My_Button_Init(&button,&Button_Struct)
}
void APP_SPI1_Init(void)
{
	//开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	//初始化引脚
	GPIO_InitTypeDef GPIO_Struct;
	//sck
	GPIO_Struct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);
	//MOSI
	GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);
	//MISO
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA,&GPIO_Struct);
	//普通IO
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Struct.GPIO_Pin = GPIO_Pin_4;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_Struct);

	//spi初始化
	SPI_InitTypeDef SPI_Struct;
	SPI_Struct.SPI_Mode = SPI_Mode_Master;
	SPI_Struct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;//72/64 1M
	SPI_Struct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_Struct.SPI_CPOL = SPI_CPOL_Low;
	SPI_Struct.SPI_DataSize = SPI_DataSize_8b;
	SPI_Struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_Struct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Struct.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1,&SPI_Struct);

	SPI_NSSInternalSoftwareConfig(SPI1,SPI_NSSInternalSoft_Set);
}


void APP_SPI_MasterTransmitReceive(SPI_TypeDef* SPIX, const uint8_t *pdataTX,  uint8_t *pdataRx, uint16_t size)
{
	if (size == 0)
	{
		return ;
	}
	//闭合spi总开关
	SPI_Cmd(SPIX,ENABLE);

	//写入第一个字节
	SPI_I2S_SendData(SPIX,pdataTX[0]);

	//读写size-1字节
	for (uint16_t i = 0; i < size - 1; i++)
	{
		while(SPI_I2S_GetFlagStatus(SPIX,SPI_I2S_FLAG_TXE) == RESET);
		SPI_I2S_SendData(SPIX,pdataTX[i + 1]);
		while(SPI_I2S_GetFlagStatus(SPIX,SPI_I2S_FLAG_RXNE) == RESET);
		pdataRx[i] = (SPIX);
	}

	//读取最后一个字节
	while(SPI_I2S_GetFlagStatus(SPIX,SPI_I2S_FLAG_RXNE) == RESET);

	pdataRx[size - 1] = SPI_I2S_ReceiveData(SPIX);

	//断开总开关
	SPI_Cmd(SPIX,DISABLE);
}

void APP_W25Q64_SaveByte(uint8_t Byte)
{
	uint8_t buffer[10];

	//1.写使能
	buffer[0] = 0x06;

	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET); //选中
	APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,1);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);


	//扇区擦除

	buffer[0] = 0x20;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;

	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);

	APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,4);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);

	//等待空闲
	while (1)
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);

		buffer[0] = 0x05;
		APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,1);

		//读状态寄存器
		buffer[0] = 0xff;
		APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,1);
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		if ((buffer[0] & 0x01) == 0)
		{
			break;
		}
	}

	//写使能
	buffer[0] = 0x06;
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);

	APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,1);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);

	//页编程
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);

	buffer[0] = 0x02;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;
	buffer[4] = Byte;
	
	APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,5);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);

	//等待空闲
	while (1)
	{
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);

		buffer[0] = 0x05;
		APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,1);

		//读状态寄存器
		buffer[0] = 0xff;
		APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,1);
		GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		if ((buffer[0] & 0x01) == 0)
		{
			break;
		}
}

uint8_t APP_W25Q64_LoadByte(void)
{
	uint8_t buffer[10];

	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);

	buffer[0] = 0x03;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;

	APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,4);

	buffer[0] = 0xff;

	APP_SPI_MasterTransmitReceive(SPI1,buffer,buffer,1);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
	return buffer[0];
}