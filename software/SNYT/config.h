// config.h
// общие определения и конфигурация переферии

#ifndef CONFIG_H_
#define CONFIG_H_
#define F_CPU 16000000UL


#include <avr/io.h>

#define EnableInterrupt		asm("sei")
#define DisableInterrupt	asm("cli")
#define NOP					asm("nop")

void BaseConfig();		// базовая конфигурация контроллера
void TimerInit();		// конфигурируем программный ШИМ и простые таймеры
void ConfigOFF();		// функция конфигурации выключенного состяния 
void WDTInit();
#endif