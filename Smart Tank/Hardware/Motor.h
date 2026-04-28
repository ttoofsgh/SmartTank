#ifndef __MOTOR_H
#define __MOTOR_H

void Motor_Init(void);
//电机驱动配置（TB6612_STBY）
void Motor_Enable(uint8_t Status);
void Motor_1_SetPWM(int8_t PWM);
void Motor_1_Brake(void);
void Motor_2_SetPWM(int8_t PWM);
void Motor_2_Brake(void);

#endif
