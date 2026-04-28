#include "stm32f10x.h"                  // Device header
#include "PID.h"
#include <math.h>

//全量式PID
void PID_Update(PID_t *p)
{
	p->Error1 = p->Error0;
	p->Error0 = p->Target - p->Actual;
	
	float C = 1.0f;
	
	//变速积分：（按需取消注释使用）
	/*C是0~1之间的小数，误差绝对值越大，C的值越小，则误差积分积累得越慢；
	式中的0.2为变速衰减系数，可自行设定其值，取值0~正无穷，
	k越大，C随误差绝对值的增大减小得越快*/
//	C = 1 / (0.2 * fabs(p->Error0 ) + 1);
	
	//计算误差积分
	p->ErrorInt += C * p->Error0;
	
	//计算PID积分项
	float IntOut = p->Ki * p->ErrorInt;
	
	//计算微分项（按需选取，若选微分先行注意更新Actual1）
	float DifOut = p->Kd * (p->Error0 - p->Error1);	//普通微分项
	//float DifOut = - p->Kd * (p->Actual - p->Actual1);	//微分先行的微分项
	//不完全微分
//	float a = 0.9;	//滤波强度系数，取值范围0~1，值越大滤波效果越强
//	float DifOut = (1 - a) * p->Kd * (p->Error0 - p->Error1) + a * DifOut;
	
	//积分限幅
	if (IntOut > p->OutMax) {IntOut = p->OutMax;}
	if (IntOut < p->OutMin) {IntOut = p->OutMin;}
	
	//PID计算
	p->Out = p->Kp * p->Error0 
		   + IntOut
		   + DifOut;
	
	//输出限幅
	if (p->Out > p->OutMax) {p->Out = p->OutMax;}
	if (p->Out < p->OutMin) {p->Out = p->OutMin;}
}
