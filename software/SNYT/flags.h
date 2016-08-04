/*
 * flags.h
 *
 * Created: 14.07.2016 1:05:38
 *  Author: 1489
 */ 


#ifndef FLAGS_H_
#define FLAGS_H_


// флаговый автомат
volatile struct
{
	_Bool min_timer: 1;
	_Bool big_timer: 1;
	_Bool mega_timer: 1;
	_Bool wdt: 1;
	//_Bool but_timer: 1;
	
	_Bool fdbck_timer: 1;
	_Bool ef_deg45: 1;
	_Bool ef_fade: 1;
	_Bool ef_ch36: 1;
	_Bool ef_shadow: 1;
	_Bool ef_jump: 1;
} flag;




#endif /* FLAGS_H_ */