// Atmega8A
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "bits_macros.h"
#include "config.h"
#include "softpwm10.h"
#include "effects.h"
#include "flags.h"
#include "snytdefs.h"
#include "port_macros.h"
#include "virtport.h"
#include "timers.h"
#include <util/delay.h>


//======================================================
// внешние переменные

// массив со знчениями ШИМ
uint16_t pwm_setting[NUM_CH];

// массив значений яркости
extern const uint16_t pwmtable_10[PWM10SIZE] PROGMEM;
extern const uint16_t pwmtable_9[PWM9SIZE] PROGMEM;

// сохраненные настройки
extern uint8_t save_program EEMEM; // номер активной программы
extern uint8_t save_deg45_time EEMEM; // тайминги программ
extern uint8_t save_ch36_time EEMEM;
extern uint8_t save_shadow_time EEMEM;
extern uint8_t save_jump_time_counter EEMEM;
extern uint8_t save_fade_time EEMEM;
extern uint8_t save_circle_time EEMEM;

extern uint8_t program;
extern uint8_t deg45_time;
extern uint8_t ch36_time;
extern uint8_t shadow_time;
extern uint8_t jump_time_counter;
extern uint8_t fade_time;
extern uint8_t circle_time;

// таймеры эффектов
extern uint16_t min_time;
extern uint32_t min_time32;
extern uint8_t big_time;

// таблицы таймингов эффектов
// extern const uint32_t timingTableDeg45[ITDEG45] PROGMEM;
// extern const uint32_t timingTableChaos36[ITCH36] PROGMEM;
// extern const uint32_t timingTableShadow[ITSHADOW] PROGMEM;
// extern const uint32_t timingTableFade[ITFADE] PROGMEM;
// extern const uint32_t timingTableJump[8][ITJUMP] PROGMEM;

extern const uint16_t timingTable_1[ITDEG45] PROGMEM;
extern const uint16_t timingTable_2[IT2] PROGMEM;
extern const uint16_t timingTable_3[ITCH36] PROGMEM;
extern const uint16_t ShadowTimingTable[ITSHADOW] PROGMEM;
extern const uint16_t FadeTimingTable[ITFADE] PROGMEM;
extern const uint16_t CircleTimingTable[ITFADE] PROGMEM;

extern const uint8_t timingTableDeg45_8[ITDEG45] PROGMEM;
extern const uint8_t timingTableChaos36_8[ITCH36] PROGMEM;
extern const uint8_t timingTableShadow_8[ITSHADOW] PROGMEM;
extern const uint16_t timingTableFade_16[ITFADE] PROGMEM;
extern const uint8_t timingTableJump_8[ITJUMP][DIRJUMP+1] PROGMEM;


//=====================================================
// глобальные переменные

#define BTNTIME 12*MS;
uint16_t but_time = BTNTIME;

#define FDBCKTIME 300*MS;
uint16_t fdbck_time = FDBCKTIME;

ISR(TIMER1_COMPA_vect)

{
	switch (program)
	{
		case OFF_NYT:
		case DEG45:
		case CHAOS36:
		case SHADOW:
		case JUMP:
			OCR1A += (uint16_t) T_PWM;
			break;
		case CIRCLE:
			if (flag.ef_fade == SET)
			{
				ISR_SoftPWM();
			}
			else
			{
				OCR1A += (uint16_t) T_PWM;
			}
			break;
		case FADE:
			ISR_SoftPWM();
			break;
	}
	
	TimerMin();
	TimerBig();
	TimerMega();
	
	// таймер горения фидбека
	if (flag.fdbck_timer == SET) {
		if (fdbck_time != 0) fdbck_time--;
		else {
			flag.fdbck_timer = UNSET;
			fdbck_time = FDBCKTIME;
		}
	}
	
	// сбрасывваем сторотожевой таймер
	asm("WDR");
}

// кнопочка переключения программ
ISR(INT0_vect)
{
	if (flag.fdbck_timer == UNSET)
	{
		flag.fdbck_timer = SET;
		if (program < NUMPROG+1) program++;
		else program = 0;
		eeprom_write_byte(&save_program, program);
		PM_WritePort(LED_PORT, 0);
	
		switch (program)
		{
			case OFF_NYT:
			case DEG45:
			case CHAOS36:
			case SHADOW:
			case JUMP:
				flag.big_timer = SET;
				break;
			case FADE:
				flag.min_timer = SET;
				break;
			case CIRCLE:
				flag.big_timer = SET;
				break;
			}
	}
	flag.fdbck_timer = SET;
}

// кнопка настройки эффекта
ISR(INT1_vect)
{
	if (flag.fdbck_timer == UNSET)
	{
		switch (program)
		{
			case OFF_NYT:
				break;
			
			case DEG45:
			if (deg45_time < (ITDEG45-1)) deg45_time++;
			else deg45_time = 0;
			eeprom_write_byte(&save_deg45_time, deg45_time);
			SetBigTimer(pgm_read_byte(&timingTableDeg45_8[deg45_time]));
			flag.fdbck_timer = SET;
			break;
			
			case FADE:
			if (fade_time < ITFADE) fade_time++;
			else fade_time = 0;
			eeprom_write_byte(&save_fade_time, fade_time);
			SetMinTimer(pgm_read_word(&timingTableFade_16[fade_time]));
			flag.fdbck_timer = SET;
			break;
			
			case CHAOS36:
			if (ch36_time < (ITCH36-1)) ch36_time++;
			else ch36_time = 0;
			eeprom_write_byte(&save_ch36_time, ch36_time);
			SetBigTimer(pgm_read_byte(&timingTableChaos36_8[ch36_time]));
			flag.fdbck_timer = SET;
			break;
			
			case SHADOW:
			if (shadow_time < (ITSHADOW-1)) shadow_time++;
			else shadow_time = 0;
			eeprom_write_byte(&save_shadow_time, shadow_time);
			SetBigTimer(pgm_read_byte(&timingTableShadow_8[shadow_time]));
			flag.fdbck_timer = SET;
			break;
			
			case JUMP:
			if (jump_time_counter < (ITJUMP-1)) jump_time_counter++;
			else jump_time_counter = 0;
			eeprom_write_byte(&save_jump_time_counter, jump_time_counter);
			flag.fdbck_timer = SET;
			break;
			
			case CIRCLE:
			if (circle_time < (ITCIRCLE-1)) circle_time++;
			else circle_time = 0;
			eeprom_write_byte(&save_circle_time, circle_time);
			SetMegaTimer(pgm_read_byte(&CircleTimingTable[circle_time]));
			flag.fdbck_timer = SET;
			break;
		}
	}
}



void ButtonsServ()
{
	if (BitIsClear(PINC, 0)) PORTD &= ~(1<<2); // запускаем прерывание INT0
	else PORTD |= (1<<2);
			
	if (BitIsClear(PINC, 1)) PORTD &= ~(1<<3); // запускаем прерывание INT1
	else PORTD |= (1<<3);
}



int main(void)
{
	program = eeprom_read_byte(&save_program);
	deg45_time = eeprom_read_byte(&save_deg45_time);
	ch36_time = eeprom_read_byte(&save_ch36_time);
	shadow_time = eeprom_read_byte(&save_shadow_time);
	jump_time_counter = eeprom_read_byte(&save_jump_time_counter);
	fade_time = eeprom_read_byte(&save_fade_time);
	circle_time = eeprom_read_byte(&save_circle_time);
	
	BaseConfig();
 	TimerInit();
	WDTInit();
 	EnableInterrupt;
	
	for(;;)
	{
		ButtonsServ();
		
		if (flag.fdbck_timer == SET) FDBCK_ON;
		else FDBCK_OFF;
		
		switch (program)
		{
			case OFF_NYT:
				PM_WritePort(LED_PORT, 0);
				break;
				
			case DEG45:
				EFF_Deg45_8(pgm_read_byte(&timingTableDeg45_8[deg45_time]));
				break;
				
			case CHAOS36:
				EFF_Chaos36_8(pgm_read_byte(&timingTableChaos36_8[ch36_time]));
				break;
				
			case SHADOW:
				EFF_Shadow_8(pgm_read_byte(&timingTableShadow_8[shadow_time]));
				break;
				
			case JUMP:
				EFF_Jump_8(jump_time_counter);
				break;
				
			case FADE:
				EFF_Fade9_16(pgm_read_word(&timingTableFade_16[fade_time]));
				break;
			
			case CIRCLE:
				EFF_Circle_8(pgm_read_byte(&CircleTimingTable[circle_time]));
				break;
		}
	}
}

