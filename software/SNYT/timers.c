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


// таймеры эффектов
uint16_t min_time = 0;
uint16_t mega_time = 0;
uint8_t big_time = 0;
uint32_t min_time32 = 0;

inline void TimerMin()
{
	if (flag.min_timer == UNSET) {
		if (min_time != 0) min_time--;
		else flag.min_timer = SET;
	}
}
void SetMinTimer(uint16_t newTime)
{
	DisableInterrupt;
	flag.min_timer = UNSET;
	min_time = newTime;
	EnableInterrupt;
}


inline void TimerBig()
{
	static uint16_t tmp_big = BGTMR;
	if (flag.big_timer == UNSET) {
		if (tmp_big!=0) tmp_big--;
		else {
			tmp_big = BGTMR;
			if (big_time != 0) big_time--;
			else flag.big_timer = SET;
		}
	}
}
void SetBigTimer(uint8_t newTime)
{
	//DisableInterrupt;
	flag.big_timer = UNSET;
	big_time = newTime;
	//EnableInterrupt;
}


inline void TimerMega()
{
	static uint16_t tmp_mega = MGTMR;
	if (flag.mega_timer == UNSET) {
		if (tmp_mega!=0) tmp_mega--;
		else {
			tmp_mega = BGTMR;
			if (mega_time != 0) mega_time--;
			else flag.mega_timer = SET;
		}
	}
}

void SetMegaTimer(uint16_t newTime)
{
	DisableInterrupt;
	flag.mega_timer = UNSET;
	mega_time = newTime;
	EnableInterrupt;
}


void TimerMin32()
{
	if (flag.min_timer == UNSET) {
		if (min_time32 != 0) min_time32--;
		else flag.min_timer = SET;
	}
}
void SetMinTimer32(uint32_t newTime)
{
	DisableInterrupt;
	flag.min_timer = UNSET;
	min_time32 = newTime;
	EnableInterrupt;
}
