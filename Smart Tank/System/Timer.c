#include "stm32f10x.h"                  // Device header

void Timer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//选择内部时钟作为时基单元时钟（此处此句可省略，因为默认是内部时钟）
	TIM_InternalClockConfig(TIM1);
	
	//时基单元初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//滤波采样频率（内部时钟分频得到）
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;	//周期，即ARR自动重装器的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;	//即PSC预分频器的值（n-1，n为分频系数）
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; //重复计时器，高级定时器需要该参数，通用定时器可直接置0
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	//TIM_TimeBaseInit()函数会自动产生一个更新事件，并置位一个更新中断标志位，故需清除标志位以免影响初始状态
	TIM_ClearFlag(TIM1, TIM_IT_Update);
	
	//中断配置
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	//NVIC配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	//启动定时器
	TIM_Cmd(TIM1, ENABLE);
}

//TIM1中断函数模板（粘贴到需要用的地方）
//void TIM1_UP_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//	{
//		
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}
