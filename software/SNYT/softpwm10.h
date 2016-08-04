// Soft PWM 10 bit


#ifndef SOFTPWM10_H_
#define SOFTPWM10_H_

#include <avr/io.h>
#include "config.h"

// иницилизации переферии для ШИМ

#define OUT_PORT1		PORTD
#define OUT_PORT2		PORTB
//#define OUT_DIR			DDRB
#define NUM_CH			8	// количество каналов ШИМ
#define LED1			(1<<5) //  PD5
#define LED2			(1<<6) // PD6
#define LED3			(1<<7) // PD7

#define LED4			(1<<0) // PB0
#define LED5			(1<<1) // PB1
#define LED6			(1<<2) // PB2 
#define LED7			(1<<3) // PB3
#define LED8			(1<<4) // PB4


// #define LED1		(1<<0) //  PD5
// #define LED2		(1<<1) // PD6
// #define LED3		(1<<2) // PD7
// #define LED4		(1<<3) // PB0
// #define LED5		(1<<4) // PB1
// #define LED6		(1<<5) // PB2
// #define LED7		(1<<6) // PB3
// #define LED8		(1<<7) // PB4


// 10 бит
#define F_PWM			60UL		// Частота ШИМ в Гц
#define PWM_STEPS		512			// количество шагов ШИМ (512 - 9 бит, 1024 - 10 бит)
#define T_PWM			(F_CPU / (F_PWM * PWM_STEPS)) // значение для регистра OCR, а так же количество тактов повторения прерывания

// 9 бит
// #define F_PWM			90UL
// #define PWM_STEPS		512
// #define T_PWM			(F_CPU / (F_PWM * PWM_STEPS))

#define PWM10SIZE 128
#define PWM9SIZE 128

void ISR_SoftPWM();
void ISR_SoftPWM1();

#endif /* SOFTPWM10_H_ */