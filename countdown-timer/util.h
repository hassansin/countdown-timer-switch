/*
 * util.h
 *
 * Created: 6/21/2020 4:16:36 PM
 *  Author: Hassansin
 */ 


#ifndef UTIL_H_
#define UTIL_H_

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

#include <avr/sfr_defs.h>

#define set_bit(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define clear_bit(sfr, bit)   (_SFR_BYTE(sfr) &= ~_BV(bit))
#define rotary_constrain(v,min,max) ((v)<(min)?(max):(v)>(max)?(min):(v))
#define constrain(v,min,max) ((v)<(min)?(min):(v)>(max)?(max):(v))

#endif /* UTIL_H_ */