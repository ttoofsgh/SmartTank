#ifndef __PWM_H
#define __PWM_H

void PWM_Init(void);
//设置CCR的值（通道1）
void PWM_SetCompare1(uint16_t Compare);
//设置CCR的值（通道2）
void PWM_SetCompare2(uint16_t Compare);

#endif
