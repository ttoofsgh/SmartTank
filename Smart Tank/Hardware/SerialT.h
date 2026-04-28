#ifndef __SERIALT_H
#define __SERIALT_H

#include <stdio.h>

void SerialT_Init(void);
void SerialT_SendByte(uint8_t Byte);
void SerialT_SendArray(uint8_t *Array, uint16_t Length);
void SerialT_SendString(char *String);
void SerialT_SendNumber(uint32_t Number, uint8_t Length);
void SerialT_Printf(char *format, ...);

#endif
