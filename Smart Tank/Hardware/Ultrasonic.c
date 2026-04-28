#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint16_t Echo_us = 0;

void Ultrasonic_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	//超声波ECHO
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	//TRIG
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//配置AFIO外部中断的引脚选择
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
	
	//EXTI初始化
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure);
	
	//NVIC中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//NVIC初始化
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

//触发检测
void Ultrasonic_Trig(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_12);
	Delay_us(15);
	GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

//获取距离（单位cm）
uint8_t Ultrasonic_Get(void)
{
	uint8_t Distance = Echo_us / 30 / 2;	//1cm约30微秒，得到的是往返的距离故需除以2
	Echo_us = 0;
	
	return Distance;
}

//中断函数（检测障碍物距离）
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line11) == SET)
	{
		while ((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == Bit_SET)
			   && (Echo_us <= 1800))	//超过1800微秒（往返约60cm）退出检测
		{
			Delay_us(30);
			Echo_us += 30;
		}
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}
