// config.h
// общие определения и конфигурация переферии

#ifndef SNYTDEFS_H_
#define SNYTDEFS_H_
#include <avr/io.h>

// цветовые каналы
#define blue1	5
#define green1	6
#define yellow1	7
#define red1	0
#define blue2	1
#define green2	2
#define yellow2	3
#define red2	4

// включаем и  выключаем цветовой канал
#define on(x)		if(x>4){PORTD|=(1<<x);}else{PORTB|=(1<<x);}
#define off(x)		if(x>4){PORTD&=~(1<<x);}else{PORTB&=~(1<<x);}

// включаем и выключаем светодиод индикации обратной связи
#define FDBCK_ON	PORTD|=(1<<4)
#define FDBCK_OFF	PORTD&=~(1<<4)

#define SET 1
#define UNSET 0


#endif