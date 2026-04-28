#ifndef __PID_H
#define __PID_H

typedef struct {
	float Target;	//目标值
	float Actual;	//实际值
	float Actual1;	//上次实际值
	float Out;		//输出值
	
	float Kp;
	float Ki;
	float Kd;
	
	float Error0;		//本次误差
	float Error1;		//上次误差
	float ErrorInt;		//误差积分
	
	//输出限幅
	float OutMax;
	float OutMin;
} PID_t;

void PID_Update(PID_t *p);

#endif
