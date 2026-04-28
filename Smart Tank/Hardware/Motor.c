#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10;	//M2以及TB6612_STBY
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET);	//电机驱动初始开启
	
	PWM_Init();
}

//电机驱动配置（TB6612_STBY）
void Motor_Enable(uint8_t Status)
{
	if (Status)
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_SET);
	}
	else
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_10, Bit_RESET);
	}
	
}

void Motor_1_SetPWM(int8_t PWM)
{
	if (PWM >= 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		PWM_SetCompare2(PWM);
	}else
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		PWM_SetCompare2(-PWM);
	}
}

void Motor_1_Brake(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1);
}

void Motor_2_SetPWM(int8_t PWM)
{
	if (PWM >= 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		PWM_SetCompare1(PWM);
	}else
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare1(-PWM);
	}
}

void Motor_2_Brake(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5);
}
