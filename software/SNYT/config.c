#include <avr/io.h>
#include "config.h"
#include "softpwm10.h"

void BaseConfig()
{
	DDRB |= (1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0); // ������ 4-8 �� �����
	PORTB &= ~(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0); // ��� �� �����		

	DDRD |= (1<<5)|(1<<6)|(1<<7); // ������ 1-3 �� �����
	PORTD &= ~(1<<5)|(1<<6)|(1<<7); // ��� �� �����
	
	DDRD |= (1<<4);	// ����� PD4 �� ����� (��������� ��������� �������)
	PORTD &= ~(1<<4); // ��� ����� ��������
	
	PORTB |= (1<<5); // ����������� ���������� ���������� ����� ������
	PORTD |= (1<<0)|(1<<1); 
	PORTC |= (1<<2)|(1<<3)|(1<<4)|(1<<5);

	DDRD |= ((1<<2)|(1<<3)); // ����� INT0 � INT1 �� �����
	PORTD |= (1<<2)|(1<<3);	 // ������ ��������� 1
	
	DDRC &= ~(1<<0)|(1<<1); // ����� � �������� �� ����, PC0 � PC1
	PORTC |= (1<<0)|(1<<1); // ����������� ����������, �� ������ 1
	
	#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
	MCUCR |= (1<<ISC11)|(1<<ISC01);		// ���������� �� ���������� ������ �� INT0 � INT1
	MCUCR &= ~((1<<ISC10)|(1<<ISC00));
	GICR |= (1<<INT0)|(1<<INT1);		// �������� ��� ������� ���������� INT0 � INT1
	
	#elif defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
	EICRA |= (1<<ISC11)|(1<<ISC01);		// ���������� �� ���������� ������ �� INT0 � INT1
	EICRA &= ~((1<<ISC10)|(1<<ISC00));
	EIMSK |= (1<<INT0)|(1<<INT1);		// �������� ��� ������� ���������� INT0 � INT1
	#endif
}

void TimerInit()
{
	TCCR1B = (1 << CS10);			// ������ �������� �� ������������ �������
	#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
	TIMSK |= (1<<OCIE1A);			// ���������� �� ����������
	#elif defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
	TIMSK1 |= (1<<OCIE1A);		// ���������� �� ����������
	#endif
}

void WDTInit()
{
	WDTCR = 0b00001100;
}

void ConfigOFF()
{
	#if defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
	// ��������� �������
	TIMSK &= ~(1<<OCIE1A);
	
	#elif defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__)
	// ��������� �������
	TIMSK0 &= ~((1<<TOIE0)|(1<<OCIE0A)|(1<<OCIE0B));
	TIMSK1 &= ~((1<<TOIE1)|(1<<OCIE1A)|(1<<OCIE1B)|(1<<ICIE1));
	TIMSK2 &= ~((1<<TOIE2)|(1<<OCIE2A)|(1<<OCIE2B));
	#endif
}
