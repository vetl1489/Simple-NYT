// Soft PWM 10 bit

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "softpwm10.h"
#include "config.h"
#include "snytdefs.h"

// массив со знчениями ШИexternМ
extern uint16_t pwm_setting[NUM_CH];
// исходный массив значений яркости
// const uint16_t pwmtable_10[64] PROGMEM = { 0, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5,
// 	5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 15, 17,
// 	19, 21, 23, 26, 29, 32, 36, 40, 44, 49, 55,
// 	61, 68, 76, 85, 94, 105, 117, 131, 146, 162,
// 	181, 202, 225, 250, 279, 311, 346, 386, 430,
// 479, 534, 595, 663, 739, 824, 918, 1023 };


const uint16_t pwmtable_10[PWM10SIZE] PROGMEM = { 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 28, 29, 30, 32, 34, 35, 37, 39, 41, 43, 45, 47, 49, 51, 54, 57, 59, 62, 65, 68, 72, 75, 79, 83, 87, 91, 95, 100, 105, 110, 115, 121, 127, 133, 139, 146, 153, 161, 168, 177, 185, 194, 204, 214, 224, 235, 246, 258, 271, 284, 298, 312, 327, 343, 360, 377, 396, 415, 435, 456, 478, 502, 526, 552, 579, 607, 636, 667, 699, 733, 769, 807, 846, 887, 930, 975, 1023 };


// #define PWM9SIZE 64
// const uint16_t pwmtable_9[PWM9SIZE]  PROGMEM = { 0, 1, 2, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 10, 10, 11, 12, 13, 15, 16, 17, 19, 21, 23, 25, 27, 29, 32, 35, 38, 41, 45, 49, 54, 59, 64, 70, 76, 83, 90, 99, 108, 117, 128, 139, 152, 166, 181, 197, 215, 235, 256, 279, 304, 332, 362, 394, 430, 469, 511 };

#define PWM9SIZE 128
const uint16_t pwmtable_9[PWM9SIZE] PROGMEM = {	0, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 10, 10, 10, 11, 11, 12, 12, 13, 14, 14, 15, 15, 16, 17, 18, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 35, 36, 38, 39, 41, 43, 45, 47, 49, 51, 53, 55, 58, 60, 63, 66, 69, 72, 75, 78, 81, 85, 89, 93, 97, 101, 105, 110, 115, 120, 125, 130, 136, 142, 148, 155, 161, 168, 176, 183, 191, 200, 209, 218, 227, 237, 247, 258, 269, 281, 293, 306, 320, 334, 348, 363, 379, 396, 413, 431, 450, 469, 490, 511 };
	
inline void ISR_SoftPWM1(){	
	static uint16_t pwm_cnt = 0;
	uint8_t tmp1 = 0;
	uint8_t tmp2 = 0;
	OCR1A += (uint16_t) T_PWM;
	if(pwm_setting[0] > pwm_cnt) tmp1 |= LED1;
	if(pwm_setting[1] > pwm_cnt) tmp1 |= LED2;
	if(pwm_setting[2] > pwm_cnt) tmp1 |= LED3;
	if(pwm_setting[3] > pwm_cnt) tmp2 |= LED4;
	if(pwm_setting[4] > pwm_cnt) tmp2 |= LED5;
	if(pwm_setting[5] > pwm_cnt) tmp2 |= LED6;
	if(pwm_setting[6] > pwm_cnt) tmp2 |= LED7;
	if(pwm_setting[7] > pwm_cnt) tmp2 |= LED8;
	OUT_PORT1 = tmp1;
	OUT_PORT2 = tmp2;
	if(pwm_cnt == (PWM_STEPS - 1)) pwm_cnt = 0;
	else pwm_cnt++;
}

inline void ISR_SoftPWM(){
	static uint16_t pwm_cnt = 0;
	PORTD &= ~(LED1|LED2|LED3);
	PORTB &= ~(LED4|LED5|LED6|LED7|LED8);
	OCR1A += (uint16_t) T_PWM;
	if(pwm_setting[blue1] > pwm_cnt) PORTD |= LED1;
	if(pwm_setting[green1] > pwm_cnt) PORTD |= LED2;
	if(pwm_setting[yellow1] > pwm_cnt) PORTD |= LED3;
	if(pwm_setting[red1] > pwm_cnt) PORTB |= LED4;
	if(pwm_setting[blue2] > pwm_cnt) PORTB |= LED5;
	if(pwm_setting[green2] > pwm_cnt) PORTB |= LED6;
	if(pwm_setting[yellow2] > pwm_cnt) PORTB |= LED7;
	if(pwm_setting[red2] > pwm_cnt) PORTB |= LED8;
	if(pwm_cnt == (PWM_STEPS - 1)) pwm_cnt = 0;
	else pwm_cnt++;
}