/*
 * virtport.h
 *
 * Created: 14.07.2016 19:59:12
 *  Author: 1489
 */ 


#ifndef VIRTPORT_H_
#define VIRTPORT_H_

#include <avr/io.h>
#include "port_macros.h"

//определяем виртуальный порт
#define LED_PORT LED, F, _VIRT
//определяем выводы виртуального порта
#define LED_0  D, 5, _HI
#define LED_1  D, 6, _HI
#define LED_2  D, 7, _HI
#define LED_3  B, 0, _HI
#define LED_4  B, 1, _HI
#define LED_5  B, 2, _HI
#define LED_6  B, 3, _HI
#define LED_7  B, 4, _HI

#endif /* VIRTPORT_H_ */