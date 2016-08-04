/*
 * timers.h
 *
 * Created: 16.07.2016 20:43:14
 *  Author: 1489
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_
#include <avr/io.h>


#define BGTMR 2000
#define MGTMR 65000
#define MS ((F_CPU/T_PWM)/1000)
//#define MS 62

void TimerMin();
void SetMinTimer(uint16_t newTime);

void TimerBig();
void SetBigTimer(uint8_t newTime);

void TimerMega();
void SetMegaTimer(uint16_t newTime);

void TimerMin32();
void SetMinTimer32(uint32_t newTime);

#endif /* TIMERS_H_ */