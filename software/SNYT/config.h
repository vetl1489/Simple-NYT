// config.h
// ����� ����������� � ������������ ���������

#ifndef CONFIG_H_
#define CONFIG_H_
#define F_CPU 16000000UL


#include <avr/io.h>

#define EnableInterrupt		asm("sei")
#define DisableInterrupt	asm("cli")
#define NOP					asm("nop")

void BaseConfig();		// ������� ������������ �����������
void TimerInit();		// ������������� ����������� ��� � ������� �������
void ConfigOFF();		// ������� ������������ ������������ �������� 
void WDTInit();
#endif