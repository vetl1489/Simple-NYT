#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "bits_macros.h"
#include "config.h"
#include "effects.h"
#include "softpwm10.h"
#include "flags.h"
#include "port_macros.h"
#include "virtport.h"
#include "timers.h"

//===========================================================
// внешние глобальные переменные
extern uint16_t pwm_setting[NUM_CH];
extern const uint16_t pwmtable_10[PWM10SIZE] PROGMEM;
extern const uint16_t pwmtable_9[PWM9SIZE] PROGMEM;

//===========================================================
// √лобальные переменные NYT

uint8_t save_program EEMEM = 1; // номер активной программы

// сохраненные тайминги программ
uint8_t save_deg45_time EEMEM = 0;
uint8_t save_ch36_time EEMEM = 0;
uint8_t save_shadow_time EEMEM = 0;
uint8_t save_jump_time_counter EEMEM = 0;
uint8_t save_fade_time EEMEM = 0;
uint8_t save_circle_time EEMEM = 0;

uint8_t program = 0;
uint8_t deg45_time = 0;
uint8_t ch36_time = 0;
uint8_t shadow_time = 0;
uint8_t jump_time_counter = 0;
uint8_t fade_time = 0;
uint8_t circle_time = 0;


// таймеры эффектов
extern uint16_t min_time;
extern uint8_t big_time;

// тайминги каждого эффекта
// const uint32_t timingTableDeg45[ITDEG45] PROGMEM = {24800, 37200, 55800, 83700, 125550};
// const uint32_t timingTableChaos36[ITCH36] PROGMEM = {24800, 37200, 55800, 83700, 125550};
// const uint32_t timingTableShadow[ITSHADOW] PROGMEM = {24800, 37200, 55800, 83700, 125550};
// const uint32_t timingTableFade[ITFADE] PROGMEM = {150, 258, 444, 765, 1317, 2265};
// const uint32_t timingTableJump[8][ITJUMP] PROGMEM = {
// 	{12448, 16704, 22400, 30048, 40320},
// 	{9280, 12448, 16704, 22400, 30048},
// 	{6912, 9280, 12448, 16704, 22400},
// 	{5152, 6912, 9280, 12448, 16704},
// 	{3840, 5152, 6912, 9280, 12448},
// 	{5152, 6912, 9280, 12448, 16704},
// 	{6912, 9280, 12448, 16704, 22400},
// 	{9280, 12448, 16704, 22400, 30048}
// };


const uint8_t timingTableDeg45_8[ITDEG45] PROGMEM = {4, 6, 9, 14, 20};
const uint8_t timingTableChaos36_8[ITCH36] PROGMEM = {4, 6, 9, 14, 20};
const uint8_t timingTableShadow_8[ITSHADOW] PROGMEM = {4, 6, 9, 14, 20};
const uint16_t timingTableFade_16[ITFADE] PROGMEM = {150, 258, 444, 765, 1317, 2265};
const uint8_t timingTableJump_8[ITJUMP][DIRJUMP+1] PROGMEM = {
	{5, 4, 3, 2, 1, 2, 3, 4},
	{10, 8, 6, 4, 2, 4, 6, 8},
	{15, 12, 9, 6, 3, 6, 9, 12},
	{20, 16, 12, 8, 4, 8, 12, 16}
};
const uint16_t CircleTimingTable[ITCIRCLE] PROGMEM = {2000,4000,6000};
//2000- 15 сек

// const uint16_t timingTable_1[ITDEG45] PROGMEM = {4000, 6000, 9000, 13500, 19500};
// const uint16_t timingTable_2[IT2] PROGMEM = {500, 1000, 2000};
// const uint16_t timingTable_3[ITCH36] PROGMEM = {400, 600, 900, 1350, 1950};
// const uint16_t ShadowTimingTable[ITSHADOW] PROGMEM = {400, 600, 900, 1350, 1950};
// //const uint16_t FadeTimingTable[ITFADE] PROGMEM = {100, 173, 299, 518, 896, 1550};
// const uint16_t FadeTimingTable[ITFADE] PROGMEM = {50, 87, 150, 259, 448, 775};
// const uint8_t CircleTimingTable[ITCIRCLE] PROGMEM = {30,50,80};



// ===========================================================
// ƒва цикла смещенных относительно друг друга на 45 градусов
// ===========================================================

#define DEG45SIZE 8
const uint8_t deg45List[DEG45SIZE] PROGMEM = {0X11, 0X12, 0X22, 0X24, 0X44, 0X48, 0X88, 0X81};
void EFF_Deg45(uint16_t time)
{
	static uint8_t count = 0;
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (count<DEG45SIZE-1) count++;
		else count=0;
		PM_WritePort(LED_PORT, pgm_read_byte(&(deg45List[count])));
		SetMinTimer(time);
	}
}

void EFF_Deg45_32(uint32_t time)
{
	static uint8_t count = 0;
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (count<DEG45SIZE-1) count++;
		else count=0;
		PM_WritePort(LED_PORT, pgm_read_byte(&(deg45List[count])));
		SetMinTimer32(time);
	}
}

void EFF_Deg45_8(uint8_t time)
{
	static uint8_t count = 0;
	if (flag.big_timer == SET)
	{
		flag.big_timer = UNSET;
		if (count<DEG45SIZE-1) count++;
		else count=0;
		PM_WritePort(LED_PORT, pgm_read_byte(&(deg45List[count])));
		SetBigTimer(time);
	}
}



// ===========================================================
// ’аотическое перемигивание двух каналов
// ===========================================================
#define EFF36SIZE 36
const uint8_t chaos36List[EFF36SIZE] PROGMEM = {0X12, 0X48, 0X24, 0X82, 0X41, 0X18, 0X84, 0X12, 0X28, 0X42, 0X81, 0X14, 0X48, 0X24, 0X41, 0X82, 0X21, 0X18, 0X41, 0X28, 0X12, 0X81, 0X24, 0X48, 0X18, 0X41, 0X82, 0X24, 0X48, 0X12, 0X84, 0X42, 0X81, 0X12, 0X28, 0X41}; // массив хаотического перемигивани€
void EFF_Chaos36(uint16_t time)
{
	static uint8_t count = 0;
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (count<EFF36SIZE-1) count++;
		else count=0;
		PM_WritePort(LED_PORT, pgm_read_byte(&(chaos36List[count])));
		SetMinTimer(time);
	}
}

void EFF_Chaos36_32(uint32_t time)
{
	static uint8_t count = 0;
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (count<EFF36SIZE-1) count++;
		else count=0;
		PM_WritePort(LED_PORT, pgm_read_byte(&(chaos36List[count])));
		SetMinTimer32(time);
	}
}

void EFF_Chaos36_8(uint8_t time)
{
	static uint8_t count = 0;
	if (flag.big_timer == SET)
	{
		flag.big_timer = UNSET;
		if (count<EFF36SIZE-1) count++;
		else count=0;
		PM_WritePort(LED_PORT, pgm_read_byte(&(chaos36List[count])));
		SetBigTimer(time);
	}
}



// ===========================================================
//  Ѕегуща€ тень
// ===========================================================
#define SHADOWSIZE 4
const uint8_t shadowList[SHADOWSIZE] PROGMEM = {0XEE, 0XDD, 0XBB, 0X77};
void EFF_Shadow(uint16_t time)
{
	static uint8_t count = 0;
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (count<SHADOWSIZE-1) count++;
		else count=0;
		PM_WritePort(LED_PORT, pgm_read_byte(&(shadowList[count])));
		SetMinTimer(time);
	}
}

void EFF_Shadow_32(uint32_t time)
{
	static uint8_t count = 0;
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (count<SHADOWSIZE-1) count++;
		else count=0;
		PM_WritePort(LED_PORT, pgm_read_byte(&(shadowList[count])));
		SetMinTimer32(time);
	}
}

void EFF_Shadow_8(uint8_t time)
{
	static uint8_t count = 0;
	if (flag.big_timer == SET)
	{
		flag.big_timer = UNSET;
		if (count<SHADOWSIZE-1) count++;
		else count=0;
		PM_WritePort(LED_PORT, pgm_read_byte(&(shadowList[count])));
		SetBigTimer(time);
	}
}



// ===========================================================
//  "ѕерескоки"
// ===========================================================
#define JUMPSIZE 56
#define DIRJUMP 7
const uint8_t jumpList[JUMPSIZE] PROGMEM = {
	0x1, 0x11, 0x13, 0x33, 0x37, 0x77, 0x7f, 0xff, 0x7f, 0x77, 0x37, 0x33, 0x23, 0x22, 0x2, 0x22, 0x26, 0x66, 0x6e, 0xee, 0xef, 0xff, 0x7f, 0x77, 0x57, 0x55, 0x45, 0x44, 0x4, 0x44, 0x4c, 0xcc, 0xcd, 0xdd, 0xdf, 0xff, 0xef, 0xee, 0xce, 0xcc, 0x8c, 0x88, 0x8, 0x88, 0x89, 0x99, 0x9b, 0xbb, 0xbf, 0xff, 0x7f, 0x77, 0x37, 0x33, 0x13, 0x11	
};
/*
void EFF_Jump(uint8_t time_counter)
{
	static uint8_t count = 0; // считаем текущий режим мигани€ 
	static uint8_t direction = 0;

	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (count<7) direction++;
		if ( count>=7 && count<14) direction--;
		if ( count>=14 && count<21) direction++;
		if ( count>=21 && count<28) direction--;
		if ( count>=28 && count<35) direction++;
		if ( count>=35 && count<42) direction--;
		if ( count>=42 && count<49) direction++;
		if ( count>=49) direction--;
		
		if (count < (JUMPSIZE-1)) count++;
		else count = 0;
		
		PM_WritePort(LED_PORT, pgm_read_byte(jumpList + count));
		SetMinTimer(pgm_read_word(&(timingTable_5[direction][time_counter])));
	}
}
*/
/*
void EFF_Jump_32(uint8_t time_counter)
{
		static uint8_t count = 0; // считаем текущий режим мигани€
		static uint8_t direction = 0;

		if (flag.min_timer == SET)
		{
			flag.min_timer = UNSET;
			if ( count<7 ) direction++;
			if ( count>=7 && count<14 ) direction--;
			if ( count>=14 && count<21 ) direction++;
			if ( count>=21 && count<28 ) direction--;
			if ( count>=28 && count<35 ) direction++;
			if ( count>=35 && count<42 ) direction--;
			if ( count>=42 && count<49 ) direction++;
			if ( count>=49 ) direction--;
			
			if (count < (JUMPSIZE-1)) count++;
			else count = 0;
			
			PM_WritePort(LED_PORT, pgm_read_byte(jumpList + count));
			SetMinTimer32(pgm_read_word(&(timingTableJump[direction][time_counter])));
		}
}
*/
void EFF_Jump_8(uint8_t time_counter)
{
	static uint8_t count = 0; // считаем текущий режим мигани€
	static uint8_t direction = 0;

	if (flag.big_timer == SET)
	{
		flag.big_timer = UNSET;
		
		if ( count<7 ) direction++;
		if ( count>=7 && count<14 ) direction--;
		if ( count>=14 && count<21 ) direction++;
		if ( count>=21 && count<28 ) direction--;
		if ( count>=28 && count<35 ) direction++;
		if ( count>=35 && count<42 ) direction--;
		if ( count>=42 && count<49 ) direction++;
		if ( count>=49 ) direction--;
		
		if (count < (JUMPSIZE-1)) count++;
		else count = 0;
		
		PM_WritePort(LED_PORT, pgm_read_byte(jumpList + count));
		SetBigTimer(pgm_read_byte(&(timingTableJump_8[time_counter][direction])));
	}
}


// ===========================================================
//  Fade
// ===========================================================
void EFF_Fade10(uint16_t time)
{
	uint8_t th = 0; // индекс массива включени€ следующего канала, пока горит предыдущий
	static uint8_t direction = 0; // направление, увеличение/уменьшение €ркости
	static uint8_t count_blue = 0; // счетчики €ркости каждого канала(индекс массива)
	static uint8_t count_green = 0;
	static uint8_t count_yellow = 0;
	static uint8_t count_red = 0;
	
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (direction == 0)
		{
			if (count_red>0) count_red--;
			
			if (count_blue<PWM10SIZE-1) count_blue++;
			else direction=1;
			DisableInterrupt;
			pwm_setting[red1] = pgm_read_word(&pwmtable_10[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_10[count_red]);
			pwm_setting[blue1] = pgm_read_word(&pwmtable_10[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_10[count_blue]);
			EnableInterrupt;
		}
		if (direction == 1)
		{
			if (count_blue>th) count_blue--;
			else direction = 2;
			DisableInterrupt;
			pwm_setting[blue1] = pgm_read_word(&pwmtable_10[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_10[count_blue]);
			EnableInterrupt;
		}

		if (direction == 2)
		{
			if (count_blue>0) count_blue--;

			if (count_green<PWM10SIZE-1) count_green++;
			else direction = 3;
			DisableInterrupt;
			pwm_setting[blue1] = pgm_read_word(&pwmtable_10[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_10[count_blue]);
			pwm_setting[green1] = pgm_read_word(&pwmtable_10[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_10[count_green]);
			EnableInterrupt;
		}
		if (direction == 3)
		{
			if (count_green>th) count_green--;
			else direction = 4;
			DisableInterrupt;
			pwm_setting[green1] = pgm_read_word(&pwmtable_10[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_10[count_green]);
			EnableInterrupt;
		}
		
		if (direction == 4)
		{
			if (count_green>0) count_green--;
			if (count_yellow<PWM10SIZE-1) count_yellow++;
			else direction = 5;
			DisableInterrupt;
			pwm_setting[green1] = pgm_read_word(&pwmtable_10[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_10[count_green]);
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_10[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_10[count_yellow]);
			EnableInterrupt;
		}
		if (direction == 5)
		{
			if (count_yellow>th) count_yellow--;
			else direction = 6;
			DisableInterrupt;
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_10[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_10[count_yellow]);
			EnableInterrupt;
		}
		
		if (direction == 6)
		{
			if (count_yellow>0) count_yellow--;
			if (count_red<PWM10SIZE-1) count_red++;
			else direction = 7;
			DisableInterrupt;
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_10[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_10[count_yellow]);
			pwm_setting[red1] = pgm_read_word(&pwmtable_10[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_10[count_red]);
			EnableInterrupt;
		}
		if (direction == 7)
		{
			if (count_red>th) count_red--;
			else direction = 0;
			DisableInterrupt;
			pwm_setting[red1] = pgm_read_word(&pwmtable_10[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_10[count_red]);
			EnableInterrupt;
		}

		SetMinTimer(time);
	}
}

// ===========================================================
//  Fade
// ===========================================================
void EFF_Fade9(uint16_t time)
{
	uint8_t th = 0; // индекс массива включени€ следующего канала, пока горит предыдущий
	static uint8_t direction = 0; // направление, увеличение/уменьшение €ркости
	static uint8_t count_blue = 0; // счетчики €ркости каждого канала(индекс массива)
	static uint8_t count_green = 0;
	static uint8_t count_yellow = 0;
	static uint8_t count_red = 0;
	
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (direction == 0)
		{
			if (count_red>0) count_red--;
			
			if (count_blue<PWM9SIZE-1) count_blue++;
			else direction=1;
			DisableInterrupt;
			pwm_setting[red1] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[blue1] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_9[count_blue]);
			EnableInterrupt;
		}
		if (direction == 1)
		{
			if (count_blue>th) count_blue--;
			else direction = 2;
			DisableInterrupt;
			pwm_setting[blue1] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_9[count_blue]);
			EnableInterrupt;
		}

		if (direction == 2)
		{
			if (count_blue>0) count_blue--;

			if (count_green<PWM9SIZE-1) count_green++;
			else direction = 3;
			DisableInterrupt;
			pwm_setting[blue1] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[green1] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_9[count_green]);
			EnableInterrupt;
		}
		if (direction == 3)
		{
			if (count_green>th) count_green--;
			else direction = 4;
			DisableInterrupt;
			pwm_setting[green1] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_9[count_green]);
			EnableInterrupt;
		}
		
		if (direction == 4)
		{
			if (count_green>0) count_green--;
			if (count_yellow<PWM9SIZE-1) count_yellow++;
			else direction = 5;
			DisableInterrupt;
			pwm_setting[green1] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_9[count_yellow]);
			EnableInterrupt;
		}
		if (direction == 5)
		{
			if (count_yellow>th) count_yellow--;
			else direction = 6;
			DisableInterrupt;
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_9[count_yellow]);
			EnableInterrupt;
		}
		
		if (direction == 6)
		{
			if (count_yellow>0) count_yellow--;
			if (count_red<PWM9SIZE-1) count_red++;
			else direction = 7;
			DisableInterrupt;
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_10[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_10[count_yellow]);
			pwm_setting[red1] = pgm_read_word(&pwmtable_10[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_10[count_red]);
			EnableInterrupt;
		}
		if (direction == 7)
		{
			if (count_red>th) count_red--;
			else direction = 0;
			DisableInterrupt;
			pwm_setting[red1] = pgm_read_word(&pwmtable_10[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_10[count_red]);
			EnableInterrupt;
		}

		SetMinTimer(time);
	}
}

void EFF_Fade9_32(uint32_t time)
{
	uint8_t th = 10; // индекс массива включени€ следующего канала, пока горит предыдущий
	static uint8_t direction = 0; // направление, увеличение/уменьшение €ркости
	static uint8_t count_blue = 0; // счетчики €ркости каждого канала(индекс массива)
	static uint8_t count_green = 0;
	static uint8_t count_yellow = 0;
	static uint8_t count_red = 0;
	
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (direction == 0)
		{
			if (count_red>0) count_red--;
			
			if (count_blue<PWM9SIZE-1) count_blue++;
			else direction=1;
			DisableInterrupt;
			pwm_setting[red1] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[blue1] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_9[count_blue]);
			EnableInterrupt;
		}
		if (direction == 1)
		{
			if (count_blue>th) count_blue--;
			else direction = 2;
			DisableInterrupt;
			pwm_setting[blue1] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_9[count_blue]);
			EnableInterrupt;
		}

		if (direction == 2)
		{
			if (count_blue>0) count_blue--;

			if (count_green<PWM9SIZE-1) count_green++;
			else direction = 3;
			DisableInterrupt;
			pwm_setting[blue1] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[green1] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_9[count_green]);
			EnableInterrupt;
		}
		if (direction == 3)
		{
			if (count_green>th) count_green--;
			else direction = 4;
			DisableInterrupt;
			pwm_setting[green1] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_9[count_green]);
			EnableInterrupt;
		}
		
		if (direction == 4)
		{
			if (count_green>0) count_green--;
			if (count_yellow<PWM9SIZE-1) count_yellow++;
			else direction = 5;
			DisableInterrupt;
			pwm_setting[green1] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_9[count_yellow]);
			EnableInterrupt;
		}
		if (direction == 5)
		{
			if (count_yellow>th) count_yellow--;
			else direction = 6;
			DisableInterrupt;
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_9[count_yellow]);
			EnableInterrupt;
		}
		
		if (direction == 6)
		{
			if (count_yellow>0) count_yellow--;
			if (count_red<PWM9SIZE-1) count_red++;
			else direction = 7;
			DisableInterrupt;
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[red1] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_9[count_red]);
			EnableInterrupt;
		}
		if (direction == 7)
		{
			if (count_red>th) count_red--;
			else direction = 0;
			DisableInterrupt;
			pwm_setting[red1] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_9[count_red]);
			EnableInterrupt;
		}
		SetMinTimer32(time);
	}
}

void EFF_Fade9_16(uint16_t time)
{
	uint8_t th = 10; // индекс массива включени€ следующего канала, пока горит предыдущий
	static uint8_t direction = 0; // направление, увеличение/уменьшение €ркости
	static uint8_t count_blue = 0; // счетчики €ркости каждого канала(индекс массива)
	static uint8_t count_green = 0;
	static uint8_t count_yellow = 0;
	static uint8_t count_red = 0;
	
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if (direction == 0)
		{
			if (count_red>0) count_red--;
			
			if (count_blue<PWM9SIZE-1) count_blue++;
			else direction=1;
			DisableInterrupt;
			pwm_setting[red1] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[blue1] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_9[count_blue]);
			EnableInterrupt;
		}
		else if (direction == 1)
		{
			if (count_blue>th) count_blue--;
			else direction = 2;
			DisableInterrupt;
			pwm_setting[blue1] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_9[count_blue]);
			EnableInterrupt;
		}
		else if (direction == 2)
		{
			if (count_blue>0) count_blue--;

			if (count_green<PWM9SIZE-1) count_green++;
			else direction = 3;
			DisableInterrupt;
			pwm_setting[blue1] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[blue2] = pgm_read_word(&pwmtable_9[count_blue]);
			pwm_setting[green1] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_9[count_green]);
			EnableInterrupt;
		}
		else if (direction == 3)
		{
			if (count_green>th) count_green--;
			else direction = 4;
			DisableInterrupt;
			pwm_setting[green1] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_9[count_green]);
			EnableInterrupt;
		}
		else if (direction == 4)
		{
			if (count_green>0) count_green--;
			if (count_yellow<PWM9SIZE-1) count_yellow++;
			else direction = 5;
			DisableInterrupt;
			pwm_setting[green1] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[green2] = pgm_read_word(&pwmtable_9[count_green]);
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_9[count_yellow]);
			EnableInterrupt;
		}
		else if (direction == 5)
		{
			if (count_yellow>th) count_yellow--;
			else direction = 6;
			DisableInterrupt;
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_9[count_yellow]);
			EnableInterrupt;
		}
		else if (direction == 6)
		{
			if (count_yellow>0) count_yellow--;
			if (count_red<PWM9SIZE-1) count_red++;
			else direction = 7;
			DisableInterrupt;
			pwm_setting[yellow1] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[yellow2] = pgm_read_word(&pwmtable_9[count_yellow]);
			pwm_setting[red1] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_9[count_red]);
			EnableInterrupt;
		}
		else if (direction == 7)
		{
			if (count_red>th) count_red--;
			else direction = 0;
			DisableInterrupt;
			pwm_setting[red1] = pgm_read_word(&pwmtable_9[count_red]);
			pwm_setting[red2] = pgm_read_word(&pwmtable_9[count_red]);
			EnableInterrupt;
		}
		SetMinTimer(time);
	}
}



// ===========================================================
//  ¬се по кругу
// ===========================================================
/*
void EFF_Circle(uint8_t time_eff)
{
	static uint8_t eff_counter = 1;
	if (flag.big_timer == SET)
	{
		flag.big_timer = UNSET;
		if (eff_counter < NUMPROG) eff_counter++;
		else eff_counter = 1;
		switch(eff_counter)
		{
			case DEG45:
				flag.ef_fade = UNSET;
				flag.ef_deg45 = SET;
				PM_WritePort(LED_PORT, 0);
				break;
			case CHAOS36:
				flag.ef_deg45 = UNSET;
				flag.ef_ch36 = SET;
				PM_WritePort(LED_PORT, 0);
				break;
			case SHADOW:
				flag.ef_ch36 = UNSET;
				flag.ef_shadow = SET;
				PM_WritePort(LED_PORT, 0);
				break;
			case JUMP:
				flag.ef_shadow = UNSET;
				flag.ef_jump = SET;
				PM_WritePort(LED_PORT, 0);
				break;
			case FADE:
				flag.ef_jump = UNSET;
				flag.ef_fade = SET;
				PM_WritePort(LED_PORT, 0);
				break;
		}
		SetBigTimer(time_eff);
	}	

	if (flag.ef_deg45 == SET)
	{
		//EFF_Deg45(pgm_read_word(&timingTable_1[eeprom_read_byte(&save_deg45_time)]));
		EFF_Deg45_32(pgm_read_word(&timingTableDeg45[eeprom_read_byte(&save_deg45_time)]));
	}
	if (flag.ef_ch36 == SET)
	{
		//EFF_Chaos36(pgm_read_word(&timingTable_3[eeprom_read_byte(&save_ch36_time)]));
		EFF_Chaos36_32(pgm_read_word(&timingTableChaos36[eeprom_read_byte(&save_ch36_time)]));
	}
	if (flag.ef_shadow == SET)
	{
		//EFF_Shadow(pgm_read_word(&ShadowTimingTable[eeprom_read_byte(&save_shadow_time)]));
		EFF_Shadow_32(pgm_read_word(&timingTableShadow[eeprom_read_byte(&save_shadow_time)]));
	}
	if (flag.ef_jump == SET)
	{
		//EFF_Jump(eeprom_read_byte(&save_jump_time_counter));
		EFF_Jump_32(eeprom_read_byte(&save_jump_time_counter));
	}
	if (flag.ef_fade == SET)
	{
		EFF_Fade9_32(pgm_read_word(&timingTableFade[eeprom_read_byte(&save_fade_time)]));
	}
}
*/


void EFF_Circle_8(uint8_t time_eff)
{
	static uint8_t eff_counter = 1;
	
	if (flag.mega_timer == SET)
	{
		flag.mega_timer = UNSET;
		
		if (eff_counter < NUMPROG) eff_counter++;
		else eff_counter = 1;
		
		switch(eff_counter)
		{
			case DEG45:
			flag.ef_fade = UNSET;
			flag.ef_deg45 = SET;
			PM_WritePort(LED_PORT, 0);
			break;
			
			case CHAOS36:
			flag.ef_deg45 = UNSET;
			flag.ef_ch36 = SET;
			PM_WritePort(LED_PORT, 0);
			break;
			
			case SHADOW:
			flag.ef_ch36 = UNSET;
			flag.ef_shadow = SET;
			PM_WritePort(LED_PORT, 0);
			break;
			
			case JUMP:
			flag.ef_shadow = UNSET;
			flag.ef_jump = SET;
			PM_WritePort(LED_PORT, 0);
			break;
			
			case FADE:
			flag.ef_jump = UNSET;
			flag.ef_fade = SET;
			PM_WritePort(LED_PORT, 0);
			break;
		}
		//SetBigTimer(time_eff);
		SetMegaTimer(time_eff);
	}

	if (flag.ef_deg45 == SET)
	{
		//EFF_Deg45(pgm_read_word(&timingTable_1[eeprom_read_byte(&save_deg45_time)]));
		//EFF_Deg45_8(pgm_read_byte(&timingTableDeg45_8[eeprom_read_byte(&save_deg45_time)]));
		EFF_Deg45_8(pgm_read_byte(&timingTableDeg45_8[deg45_time]));
	}
	else if (flag.ef_ch36 == SET)
	{
		//EFF_Chaos36(pgm_read_word(&timingTable_3[eeprom_read_byte(&save_ch36_time)]));
		//EFF_Chaos36_8(pgm_read_byte(&timingTableChaos36_8[eeprom_read_byte(&save_ch36_time)]));
		EFF_Chaos36_8(pgm_read_byte(&timingTableChaos36_8[ch36_time]));
	}
	else if (flag.ef_shadow == SET)
	{
		//EFF_Shadow(pgm_read_word(&ShadowTimingTable[eeprom_read_byte(&save_shadow_time)]));
		//EFF_Shadow_8(pgm_read_byte(&timingTableShadow_8[eeprom_read_byte(&save_shadow_time)]));
		EFF_Shadow_8(pgm_read_byte(&timingTableShadow_8[shadow_time]));
	}
	else if (flag.ef_jump == SET)
	{
		//EFF_Jump(eeprom_read_byte(&save_jump_time_counter));
		//EFF_Jump_8(eeprom_read_byte(&save_jump_time_counter));
		EFF_Jump_8(jump_time_counter);
	}
	else if (flag.ef_fade == SET)
	{
		//EFF_Fade9_16(pgm_read_word(&timingTableFade_16[eeprom_read_byte(&save_fade_time)]));
		EFF_Fade9_16(pgm_read_word(&timingTableFade_16[fade_time]));
	}
}


void foo3(uint32_t time)
{
	static uint8_t count = 0;
	if (flag.min_timer == SET)
	{
		flag.min_timer = UNSET;
		if(count<PWM10SIZE-1) count++;
		else count = 0;
		pwm_setting[red2] = pgm_read_word(&pwmtable_9[count]);
		SetMinTimer32(time);
	}
}