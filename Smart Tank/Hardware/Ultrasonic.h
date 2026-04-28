#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H

extern uint16_t Echo_us;

void Ultrasonic_Init(void);
void Ultrasonic_Trig(void);
uint8_t Ultrasonic_Get(void);

#endif
