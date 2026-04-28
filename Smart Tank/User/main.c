#include "stm32f10x.h"                  // Device header
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "BLE.h"
#include "Motor.h"
#include "Encoder.h"
#include "SerialT.h"
#include "Ultrasonic.h"
#include "Timer.h"
#include "PID.h"
#include "Track.h"
#include "Delay.h"
#include "Telecontrol.h"
#include "NRF24L01.h"

#define TELECONTROL_MODE  0
#define TRACK_MODE        1

//模式及通信方式（BLE：0，2.4G：1）
uint8_t Mode = 0, Comm = 0;
//主循环计时
uint32_t Interval = 0;
//电机速度
int16_t Speed1 = 0, Speed2 = 0;
//障碍物检测
uint8_t Distance = 30;
//PID内环参数
PID_t Inner1 = {
	.Kp = 0.2f,
	.Ki = 0.1f,
	.Kd = 0.2f,
	
	.OutMax = 100,
	.OutMin = -100,
};

PID_t Inner2 = {
	.Kp = 0.2f,
	.Ki = 0.1f,
	.Kd = 0.2f,
	
	.OutMax = 100,
	.OutMin = -100,
};

//PID外环参数
PID_t Outer = {
	.Kp = 110.0f,
	.Ki = 0.0f,
	.Kd = 0.0f,
	
	.Target = 0.0f,
	
	//以下两项可限制内环目标值
	.OutMax = 100,
	.OutMin = -100,
};

int main(void)
{
	Switch_Init();		//切换开关初始化
	Delay_ms(10);
	
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == 0)
	{
		Mode = TELECONTROL_MODE;
	}
	else 
	{
		Mode = TRACK_MODE;
		
		Encoder_Init();
		SerialT_Init();
		Track_Init();
		Ultrasonic_Init();
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 1)
	{
		Comm = 1;
		
		NRF24L01_Init();
	}
	else 
	{
		Comm = 0;
		
		Serial_Init();
	}
	
	Motor_Init();
	Timer_Init();
	
	while(1)
	{
		if (Mode == TELECONTROL_MODE)
		{
			if (Comm == 1)		//2.4G遥控
			{
				NRF24L01_Receive();
				Motor_1_SetPWM((int8_t)NRF24L01_RxPacket[2]);
				Motor_2_SetPWM((int8_t)NRF24L01_RxPacket[3]);
			}
			else if (Comm == 0)	//BLE遥控
			{
				if (Serial_RxFlag == 1)
				{
					int16_t LPWM_raw = 0, RPWM_raw = 0;
					int16_t LPWM = 0, RPWM = 0;
					//分割字符串（以“,”为分隔符），依次取出子串
					char *Tag = strtok(Serial_RxPacket, ",");
					if (strcmp(Tag, "joystick") == 0)
					{
						char *JoyX = strtok(NULL, ",");
						char *JoyY = strtok(NULL, ",");
						LPWM_raw = atoi(JoyY) + atoi(JoyX);
						RPWM_raw = atoi(JoyY) - atoi(JoyX);
						
						int16_t max_abs = (abs(LPWM_raw) > abs(RPWM_raw)) ? abs(LPWM_raw) : abs(RPWM_raw);

						if (max_abs > 100) 
						{
							float scale = 100.0f / max_abs;
							LPWM = (int16_t)(LPWM_raw * scale);
							RPWM = (int16_t)(RPWM_raw * scale);
						} else 
						{
							LPWM = LPWM_raw;
							RPWM = RPWM_raw;
						}
						Motor_1_SetPWM(LPWM);
						Motor_2_SetPWM(RPWM);
					}
				
					Serial_RxFlag = 0;	//清除虚拟标志位，防止数据覆盖
					
					static uint32_t lastSend = 0;
					if ((Interval - lastSend) > 40)
					{
						Serial_Printf("[display,20,20,Telecontrolling,20]\r\n");
						Serial_Printf("[display,20,40,M1:%+03d,20]\r\n", LPWM);
						Serial_Printf("[display,20,60,M2:%+03d,20]\r\n", RPWM);
						lastSend = Interval;
					}
				}
			}
		}
		else if (Mode == TRACK_MODE)
		{
			if (Comm == 1)		//2.4G
			{
				static uint32_t lastTime = 0;
				if ((Interval - lastTime) > 40)
				{
					NRF24L01_TxPacket[0] = (0x01 << 6) | Track_GetHex();
					NRF24L01_TxPacket[1] = Distance;
					NRF24L01_TxPacket[2] = Inner1.Out;
					NRF24L01_TxPacket[3] = Inner2.Out;
					
					NRF24L01_Send();
					lastTime = Interval;
				}
			}
			else if (Comm == 0)	//BLE
			{
				static uint32_t lastT = 0;
				if ((Interval - lastT) > 40)
				{
					Serial_Printf("[display,20,20,Tracking,20]\r\n");
					Serial_Printf("[display,20,40,Trac_sign:%d,20]\r\n", Track_GetNum());
					Serial_Printf("[display,20,60,Obst_dist:%d,20]\r\n", Distance);
					Serial_Printf("[display,20,80,M1:%d,20]\r\n", Inner1.Out);
					Serial_Printf("[display,20,120,M2:%d,20]\r\n", Inner2.Out);
					lastT = Interval;
				}
			}
			
			static uint32_t lastUpdate = 0;
			if ((Interval - lastUpdate) > 40)
			{
				Ultrasonic_Trig();
				Distance = Ultrasonic_Get();
				lastUpdate = Interval;
			}
			
			if (Distance <= 20)
			{
				if (Distance < 15)
				{
					Motor_1_SetPWM(-50);
					Motor_2_SetPWM(-50);
				}
				else
				{
					Motor_1_Brake();
					Motor_2_Brake();
				}
				
			}
		}
			
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) != Mode)	//模式切换
		{
			Delay_ms(20);
			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) != Mode)
			{
				//结束程序
				Motor_1_SetPWM(0);
				Motor_2_SetPWM(0);
				NVIC_SystemReset();
			}
		}
		
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) != Comm)	//无线通信方式切换
		{
			Delay_ms(20);
			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) != Comm)
			{
				//结束程序
				Motor_1_SetPWM(0);
				Motor_2_SetPWM(0);
				NVIC_SystemReset();
			}
		}
		
		
//		//外环PID调参（先将内环调好的参数写进初始化中，再注释掉内环调参代码，再进行外环调参）
//		if (Serial_RxFlag == 1)
//		{
//			//分割字符串（以“,”为分隔符），依次取出子串
//			strtok(Serial_RxPacket, ",");
//			char *Name = strtok(NULL, ",");
//			char *Value = strtok(NULL, ",");
//			if (strcmp(Name, "Kp") == 0)
//			{
//				//将字符串转换为浮点型（ASCII to Float），另atoi()可转为整型
//				Outer.Kp = atof(Value);
//			}
//			else if (strcmp(Name, "Ki") == 0)
//			{
//				Outer.Ki = atof(Value);
//			}
//			else if (strcmp(Name, "Kd") == 0)
//			{
//				Outer.Kd = atof(Value);
//			}
//			else if (strcmp(Name, "Target") == 0)
//			{
//				Outer.Target = atof(Value);
//			}
//			
//			Serial_RxFlag = 0;	//清除虚拟标志位，防止数据覆盖
//		}
//		
////		SerialT_Printf("%f,%f,%f\r\n", Outer.Target, Outer.Actual, Outer.Out);
//		Serial_Printf("[display,20,20,p:%f,20]\r\n", Outer.Kp);
//		Serial_Printf("[display,20,40,i:%f,20]\r\n", Outer.Ki);
//		Serial_Printf("[display,20,60,d:%f,20]\r\n", Outer.Kd);
////		Serial_Printf("[display,20,80,tar:%f,20]\r\n", Outer.Target);
//		Serial_Printf("[display,20,100,Num: %04d,20]\r\n", Track_GetNum());
//		Serial_Printf("[display,20,120,Devi: %04d,20]\r\n", Track_GetDevi());
//		Serial_Printf("[display,20,140,Speed1: %04d,20]\r\n", Speed1);
//		Serial_Printf("[display,20,160,Speed2: %04d,20]\r\n", Speed2);
		
		
	}
	
}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count1 = 0, Count2 = 0;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		Interval ++;
		if (Mode == TRACK_MODE)
		{
			//内环PID
			if (Distance >= 20)
			{
				Count1 ++;
				if (Count1 >= 40)
				{
					Count1 = 0;
					
					//获取速度，位置
					/*Encoder_Get()每次调用后会自动把计数值清零，故整个工程只能使用一次*/
					Speed1 = Encoder1_Get();
					Speed2 = Encoder2_Get();
					
					Inner1.Actual = Speed1;
					Inner2.Actual = Speed2;
					
					PID_Update(&Inner1);
					PID_Update(&Inner2);
					
					Motor_1_SetPWM(Inner1.Out);
					Motor_2_SetPWM(Inner2.Out);
				}
					
				//外环PID
				Count2 ++;
				if (Count2 >= 40)
				{
					Count2 = 0;
					
					Outer.Actual = Track_GetDevi();
					
					PID_Update(&Outer);

					
					Inner1.Target = 100 + Outer.Out;
					Inner2.Target = 100 - Outer.Out;

				}
			}
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
