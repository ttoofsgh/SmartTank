#include "stm32f10x.h"                  // Device header
#include "SerialT.h" 
#include <stdio.h>
#include <stdarg.h>

void SerialT_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;	//既需要发送又需要接收: USART_Mode_Tx | USART_Mode_Rx
	USART_InitStructure.USART_Parity = USART_Parity_No;	//校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStructure);
	
	USART_Cmd(USART2, ENABLE);
}

void SerialT_SendByte(uint8_t Byte)
{
	USART_SendData(USART2, Byte);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		;	//标志位无需手动清零
}

void SerialT_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i = 0;
	for (i = 0; i < Length; i ++)
	{
		SerialT_SendByte(Array[i]);
	}
}

void SerialT_SendString(char *String)
{
	uint8_t i = 0;
	for (i = 0; String[i] != '\0'; i ++)	//ASCII '\0'等价于0（0x00），即空字符（NUL）
	{
		SerialT_SendByte(String[i]);
	}
}

uint32_t SerialT_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void SerialT_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i = 0;
	for(i = 0; i < Length; i ++)
	{
		SerialT_SendByte(Number / SerialT_Pow(10, Length - i - 1) % 10 + 0x30);	//ASCII 0x30即'0'
	}
	
}

//int fputc(int ch, FILE *f)
//{
//	SerialT_SendByte(ch);
//	return ch;
//}

/*
char *format：接收格式化字符串
...:可变参数列表
*/
void SerialT_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	SerialT_SendString(String);
}
