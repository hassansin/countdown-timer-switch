/*
 * Rotary.h
 *
 * Created: 6/24/2020 11:39:46 AM
 * Author: Hassansin
 */

#ifndef ROTARY_H_
#define ROTARY_H_

#include <avr/interrupt.h>
#include <avr/io.h>

#define ROTARY_DDR DDRD
#define ROTARY_PORT PORTD
#define ROTARY_PIN PIND
#define ROTARY_DATA PD3
#define ROTARY_CLK PD2
#define ROTARY_SW PD4
#define OFF 1

volatile int8_t a0;
volatile int8_t c0;

void rotary_init(void (*cb)(int8_t));

#endif /* ROTARY_H_ */
