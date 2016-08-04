// эффекты

#ifndef EFFECTS_H_
#define EFFECTS_H_

#include <avr/io.h>
#include "config.h"
#include "softpwm10.h"
#include "flags.h"
#include "snytdefs.h"
#include "bits_macros.h"

// количество эффектов минус OFF_NYT и CIRCLE
#define NUMPROG 5
// программы
#define OFF_NYT 0
#define DEG45 1
#define CHAOS36 2
#define SHADOW 3
#define JUMP 4
#define FADE 5
#define CIRCLE 6

// количество настроек для каждого эффекта
#define ITDEG45	5
#define ITCH36 5
#define ITSHADOW 5
#define ITFADE 6

#define DIRJUMP 7
#define ITJUMP	4

#define ITCIRCLE 3

#define IT2	3


// 45deg
void EFF_Deg45(uint16_t time);
void EFF_Deg45_32(uint32_t time);

void EFF_Deg45_8(uint8_t time);

 // Chaos 36
void EFF_Chaos36(uint16_t time);
void EFF_Chaos36_32(uint32_t time);

void EFF_Chaos36_8(uint8_t time);

// бегущая тень
void EFF_Shadow(uint16_t time);	
void EFF_Shadow_32(uint32_t time);

void EFF_Shadow_8(uint8_t time);

// "перескоки"
void EFF_Jump(uint8_t time_counter);
void EFF_Jump_32(uint8_t time_counter);

void EFF_Jump_8(uint8_t time_counter);

// Fade
void EFF_Fade10(uint16_t time);
void EFF_Fade9(uint16_t time);
void EFF_Fade9_32(uint32_t time);

void EFF_Fade9_16(uint16_t time);

// все по кругу
void EFF_Circle(uint8_t time_eff);

void EFF_Circle_8(uint8_t time_eff);	

void foo3(uint32_t time);

#endif 