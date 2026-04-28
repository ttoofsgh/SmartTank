#include "stm32f10x.h"                  // Device header

void Track_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//将读取到的电平合并为一个16进制数字
uint8_t Track_GetHex(void)
{
	uint8_t Num = 0;
	Num = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) << 3
		| GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) << 2
		| GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) << 1
		| GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
	
	return Num;
}

//将4路循迹信号展示为数字（1为迹线上，0为迹线外）
uint16_t Track_GetNum(void)
{
	uint16_t Num = 0;
	switch(Track_GetHex())
	{
		case 0x00: // 0000 直行
			Num = 0;
			break;
		case 0x0F: // 1111 十字路口
			Num = 1111;
			break;
		case 0x02: // 0010 右1
			Num = 10;
			break;
		case 0x01: // 0001 右2
			Num = 1;
			break;
		case 0x03: // 0011 右直弯
			Num = 11;
			break;
		case 0x04: // 0100 左1
			Num = 100 ;
			break;
		case 0x08: // 1000 左2
			Num = 1000;
			break;
		case 0x0C: // 1100 左直弯
			Num = 1100;
			break;
		default:
			// 可选：未定义状态的处理
			break;
	}
	
	return Num;
}

//将信号转化为偏差信号（用于调控）
int8_t Track_GetDevi(void)
{
	int8_t Num = 0;
	switch(Track_GetHex())
	{
		case 0x00: // 0000 直行
			Num = 0;
			break;
		case 0x0F: // 1111 十字路口
			Num = 0;
			break;
		case 0x02: // 0010 右1
			Num = -1;
			break;
		case 0x01: // 0001 右2
			Num = -10;
			break;
		case 0x03: // 0011 右直弯
			Num = -20;
			break;
		case 0x07: // 0111 右直弯
			Num = -25;
			break;
		case 0x04: // 0100 左1
			Num = 1;
			break;
		case 0x08: // 1000 左2
			Num = 10;
			break;
		case 0x0C: // 1100 左直弯
			Num = 20;
			break;
		case 0x0E: // 1110 左直弯
			Num = 25;
			break;
		default:
			// 可选：未定义状态的处理
			break;
	}
	
	return Num;
}
