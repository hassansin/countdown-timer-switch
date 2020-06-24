/*
 * Buzzer.c
 *
 * Created: 6/24/2020 5:07:04 PM
 *  Author: Hassansin
 */ 


#include "Buzzer.h"

void buzzer_init() {
	//Configure Timer1:PWM, f = 1M/ICR1A
	BUZZER_DDR |= 1 << BUZZER;
	BUZZER_PORT |= 1<< BUZZER; // for active low buzzer, set the pin High
	TCCR1A |= 1<<WGM11; // Mode 14
	TCCR1B |= 1 <<WGM12 | 1 <<WGM13; //Mode 14
	ICR1 = 2000;
	OCR1A = 1000;
	TCCR1B |= 1 << CS10;
}

void buzzer_enable() {
	TCCR1A |= 1 <<COM1A1 | 1 << COM1A0; //inverting
}

void buzzer_disable() {
	TCCR1A &= ~(1 <<COM1A1 | 1 << COM1A0);
	BUZZER_PORT |= 1<<BUZZER;
}

void buzzer_toggle() {
	if(bit_is_set(TCCR1A, COM1A1)) {
		buzzer_disable();
		return;
	}	
	buzzer_enable();
}