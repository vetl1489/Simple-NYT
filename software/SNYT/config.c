#include <avr/io.h>
#include "config.h"
#include "softpwm10.h"

void BaseConfig()
{
	DDRB |= (1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0); // каналы 4-8 на выход
	PORTB &= ~(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0); // все по нулям		

	DDRD |= (1<<5)|(1<<6)|(1<<7); // каналы 1-3 на выход
	PORTD &= ~(1<<5)|(1<<6)|(1<<7); // все по нулям
	
	DDRD |= (1<<4);	// ножку PD4 на выход (Светодиод индикации нажатия)
	PORTD &= ~(1<<4); // Все время выключен
	
	PORTB |= (1<<5); // подтягиваем резистором оставшиеся ножки портов
	PORTD |= (1<<0)|(1<<1); 
	PORTC |= (1<<2)|(1<<3)|(1<<4)|(1<<5);

	DDRD |= ((1<<2)|(1<<3)); // ножки INT0 и INT1 на выход
	PORTD |= (1<<2)|(1<<3);	 // подаем постоянно 1
	
	DDRC &= ~(1<<0)|(1<<1); // ножки с кнопками на вход, PC0 и PC1
	PORTC |= (1<<0)|(1<<1); // подтягиваем резистором, на входах 1
	
	#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
	MCUCR |= (1<<ISC11)|(1<<ISC01);		// прерывания по спадающему фронту на INT0 и INT1
	MCUCR &= ~((1<<ISC10)|(1<<ISC00));
	GICR |= (1<<INT0)|(1<<INT1);		// включаем оба внешних прерывания INT0 и INT1
	
	#elif defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
	EICRA |= (1<<ISC11)|(1<<ISC01);		// прерывания по спадающему фронту на INT0 и INT1
	EICRA &= ~((1<<ISC10)|(1<<ISC00));
	EIMSK |= (1<<INT0)|(1<<INT1);		// включаем оба внешних прерывания INT0 и INT1
	#endif
}

void TimerInit()
{
	TCCR1B = (1 << CS10);			// Таймер работает на максимальной частоте
	#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
	TIMSK |= (1<<OCIE1A);			// прерывание по совпадению
	#elif defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
	TIMSK1 |= (1<<OCIE1A);		// прерывание по совпадению
	#endif
}

void WDTInit()
{
	WDTCR = 0b00001100;
}

void ConfigOFF()
{
	#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
	// выключаем таймеры
	TIMSK &= ~(1<<OCIE1A);
	
	#elif defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
	// выключаем таймеры
	TIMSK0 &= ~((1<<TOIE0)|(1<<OCIE0A)|(1<<OCIE0B));
	TIMSK1 &= ~((1<<TOIE1)|(1<<OCIE1A)|(1<<OCIE1B)|(1<<ICIE1));
	TIMSK2 &= ~((1<<TOIE2)|(1<<OCIE2A)|(1<<OCIE2B));
	#endif
}
