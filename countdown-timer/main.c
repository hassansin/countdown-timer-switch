/*
 * countdown-timer.c
 *
 * Created: 6/10/2020 10:55:04 PM
 * Author : Hassansin
 */ 
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/portpins.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#define DELAY_STEP 5 //minutes
#define MIN_DELAY 0 //minutes
#define MAX_DELAY 30 //minutes
#define SECONDS_PER_MIN 10 //seconds
#define EEPROM_ADDR ( uint8_t *) 0
#define DP 1<<7
#define WAIT_TIME 3 //seconds, time to wait after the changing counter 

#define ROT_ENC_REG PIND
#define ROT_ENC_DATA PD1

#define bit(b) (1<<b)
#define is_set(p, b)  ((p & 1<<b) != 0)
#define is_clear(p,b) ((p & 1<<b) == 0)
#define is_waiting() (wait_counter != 0)
#define is_off() (delay == 0 || is_waiting())

volatile uint16_t ms;
volatile uint8_t wait_counter;
volatile int8_t delay; //total timer delay in minute
volatile uint8_t seconds; //counts seconds, reset every minute
volatile uint8_t save_delay;


//uint8_t digits[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90}; // common anode
uint8_t digits[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67}; // common cathode

//check delay boundaries
int8_t validate_delay(int8_t d) {
	if(d < MIN_DELAY) {
		d = MAX_DELAY;
	} else if (d > MAX_DELAY ) {
		d = MIN_DELAY;
	}
	return d;
}

//Rotary Encoder Clock	
ISR(INT0_vect) {
	wait_counter = WAIT_TIME;
	if(is_set(ROT_ENC_REG, ROT_ENC_DATA)) {
		delay--;
	} else{
		delay++;
	}
	delay = validate_delay(delay);
	seconds = 0;	
}

void update_first_segment(uint8_t data) {
	PORTC |= 1<<PC1;
	PORTC &= ~(1<<PC0);
	PORTB = data;
}

void update_second_segment(uint8_t data) {
	PORTC |= 1<<PC0;
	PORTC &= ~(1<<PC1);
	PORTB = data;
}

void update_display(uint16_t jiffy){
	uint8_t d1 = digits[delay%10];
	uint8_t d2 = digits[delay/10%10];
	
	if (jiffy == 0 && !is_off()) {
		update_first_segment(d1 | DP);
	}else if (jiffy % 2 == 0) {
		update_first_segment(d1);
	} else if (d2 != digits[0]){
		update_second_segment(d2);		
	}
}

void delay_countdown(uint16_t  sec) {
	if(is_off()) {
		return;
	}
	seconds++;
	if(seconds == SECONDS_PER_MIN) {
		seconds = 0;		
		delay = validate_delay(delay-1);
	}
}

void wait_countdown(uint16_t sec) {
	if(!is_waiting()) {
		return;
	}
	wait_counter--;
	if(wait_counter == 0) {
		save_delay = 1;
	}
}

void at_each_jiffy(uint8_t jiffy) {
	update_display(jiffy);
}

void at_each_second(uint8_t sec) {
	delay_countdown(sec);
	wait_countdown(sec);
}

//Invoked at 1ms interval
ISR(TIMER2_COMP_vect) {
	ms++;
	if(ms == 1000) {
		ms = 0;		
	}
	if(ms % 10 == 0) {
		at_each_jiffy(ms/10);
	}
	if (ms == 0 ) {
		at_each_second(ms);
	}
}



int main(void)
{
	cli();
		
	//Read from EEROM, First read might return garbage value
	delay = validate_delay(eeprom_read_byte(EEPROM_ADDR));
	wait_counter = WAIT_TIME;
	
	//Display PORT
	DDRB = 0xFF;
	//Display Control
	DDRC |= 1<<PC0 | 1 << PC1;
	PORTC |= 1<<PC1; //turn off second display
	
	//Rotary Encoder Data PIN
	DDRD &= ~(1<<PD1);
	DDRD |= 1 << PD0; //Relay control pin
	
	//Rotary Encoder Clock PIN - INT0
	//Set INT0 Sense Control - falling edge
	MCUCR |=  1<<ISC01;
	GICR |= 1<<INT0;
	
	//Configure Timer2: 1000Hz/1ms
	TCCR2 |= bit(CS21); //prescaler 1/8
	TCCR2 |= bit(WGM21); //Mode 2: CTC
	OCR2 = 124;
	TIMSK |= bit(OCIE2); // Set interrupt on compare match
	
	
	sei();
	
    while (1) 
    {
		if(save_delay) {
			eeprom_update_byte(EEPROM_ADDR, delay);
			save_delay = 0;
		}
		
		if(is_off()) {
			PORTD |= 1 << PD0; //turn off relay
		} else {
			PORTD &= ~(1 << PD0); // turn on relay	(active low)		
		}
    }
}

