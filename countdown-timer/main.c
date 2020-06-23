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
#include "util.h"
#include "pinDefines.h"

#define DELAY_STEP 5 //minutes
#define MIN_DELAY 0 //minutes
#define MAX_DELAY 30 //minutes
#define SECONDS_PER_MIN 10 //seconds
#define EEPROM_ADDR ( uint8_t *) 0
#define DP 1<<7
#define WAIT_TIME 4 //seconds, time to wait after the changing counter 
#define ROT_ENC_DELAY_TH 1 //milliseconds to wait before registering a rotary encoder rotation

#define is_waiting() (wait_counter != 0)
#define is_off() (delay == 0 || is_waiting())

#define turnoff() (RELAY_PORT |= 1 << RELAY)
#define turnon() (RELAY_PORT &= ~(1 << RELAY))

volatile uint8_t finished;
volatile uint16_t ms;
volatile uint8_t wait_counter;
volatile int8_t delay; //total timer delay in minute
volatile uint8_t seconds; //counts seconds, reset every minute
volatile uint8_t save_delay;


//uint8_t digits[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90}; // common anode
uint8_t digits[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67}; // common cathode

void enable_buzzer() {
	TCCR1A |= 1 <<COM1A1 | 1 << COM1A0; //inverting
}

void disable_buzzer() {
	TCCR1A &= ~(1 <<COM1A1 | 1 << COM1A0);
	set_bit(BUZZER_PORT, BUZZER);
}

void toggle_buzzer() {
	if(bit_is_set(TCCR1A, COM1A1)) {
		disable_buzzer();
	} else {
		enable_buzzer();
	}
}


volatile uint8_t rot_enc_interrupt;
volatile int8_t rot_enc_interrupt_delay = ROT_ENC_DELAY_TH;

void check_rotary_encoder() {
	if(rot_enc_interrupt == 0) {
		return;
	}	
	rot_enc_interrupt_delay--;
	if(rot_enc_interrupt_delay >= 0) {
		return;
	}
	
	wait_counter = WAIT_TIME;
	finished = 0;
	seconds = 0;
	rot_enc_interrupt = 0;
	rot_enc_interrupt_delay = 0;
	
	disable_buzzer();
	
	if(bit_is_set(ROT_ENC_REG, ROT_ENC_DATA)) {
		delay--;
		} else{
		delay++;
	}
	delay = rotary_constrain(delay, MIN_DELAY, MAX_DELAY);	
}

//Rotary Encoder Clock	
ISR(INT0_vect) {
	rot_enc_interrupt = 1;
	rot_enc_interrupt_delay = ROT_ENC_DELAY_TH;
	//check_rotary_encoder();
}

void enable_first_segment(){
	PORTC |= 1<<PC1;
	PORTC &= ~(1<<PC0);
}

void enable_second_segment(){
	PORTC |= 1<<PC0;
	PORTC &= ~(1<<PC1);
}

void enable_both_segments(){
	PORTC &= ~(1<<PC0 | 1<<PC1);
}

void disable_both_segments() {
	PORTC |= 1<<PC0 | 1 <<PC1;
}

void toggle_both_segments() {
	if(bit_is_set(PORTC, PC0) || bit_is_set(PORTC, PC1)) {
		enable_both_segments();
	} else {
		disable_both_segments();
	}
}

void set_display_data(uint8_t data) {
	uint8_t temp = PORTB;
	temp &= ~0b11111000;
	temp |= ( data & 0b11111000 );
	PORTB = temp;
	
	temp = PORTD;
	temp &= ~0b11100000;
	temp |= ( data & 0b00000111 ) << 5;
	PORTD = temp;
}

void update_display(uint16_t cs){
	uint8_t d1 = digits[delay%10];
	uint8_t d2 = digits[delay/10%10];
 
	// 1 centi second = 10ms
	// Toggle segments at every 10ms
	// Show the dot point only at the beginning of a second AND when countdown timer is on
 
	if (cs == 0 && !is_off()) {
		enable_first_segment();
		set_display_data(d1 | DP);		
	} else if (cs % 2 == 0) {
		enable_first_segment();
		set_display_data(d1);
	} else if(d2 != digits[0]) {	//disable second segment if value is zero	
		enable_second_segment();
		set_display_data(d2);				
	}
}

void flush_display(uint16_t cs) {
	set_display_data(digits[0]);
	if(cs%25 == 0 ) {		
		toggle_both_segments();
	}
}

void tone(uint8_t cs) {
	if(cs%25 == 0) {
		toggle_buzzer();
	}
}

void delay_countdown(uint16_t  sec) {
	if(is_off()) {
		return;
	}
	seconds++;
	if(seconds == SECONDS_PER_MIN) {
		seconds = 0;		
		delay = rotary_constrain(delay-1, MIN_DELAY, MAX_DELAY);
	}
	if(delay == 0) {
		finished = 1;
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


void at_each_centi_second(uint8_t cs) {
	if(finished) {
		flush_display(cs);
		tone(cs);
	} else {
		update_display(cs);	
	}
}

void at_each_second(uint8_t sec) {
	delay_countdown(sec);
	wait_countdown(sec);
}

void setup_pwm() {
	//Configure Timer1:PWM, f = 1M/ICR1A
	DDRB |= 1 << PB1;
	PORTB |= 1<<PB1; // for active low buzzer, set the pin High
	TCCR1A |= 1<<WGM11; // Mode 14
	TCCR1B |= 1 <<WGM12 | 1 <<WGM13; //Mode 14
	ICR1 = 2000;
	OCR1A = 1000;
	TCCR1B |= 1 << CS10;
}

//Invoked at 1ms interval
ISR(TIMER2_COMP_vect) {
	ms++;
	if(ms == 1000) {
		ms = 0;
	}
	check_rotary_encoder();
	if(ms % 10 == 0) {
		at_each_centi_second(ms/10);
	}
	if (ms == 0 ) {
		at_each_second(ms);
	}
}

//initialize relay control pin
void relay_init(){
	set_bit(RELAY_DDR, RELAY);
	turnoff();
}

int main(void)
{
	cli();	
	//Read from EEROM, First read might return garbage value
	delay = rotary_constrain(eeprom_read_byte(EEPROM_ADDR), MIN_DELAY, MAX_DELAY);
	wait_counter = 2*WAIT_TIME;
	
	//Display PORT
	DDRB |= 0b11111000;
	DDRD |= 0b11100000;
	
	//Display Control
	DDRC |= 1<<PC0 | 1 << PC1;
	PORTC |= 1<<PC1; //turn off second display
	
	//Rotary Encoder Data PIN
	clear_bit(DDRD, PD1);
	DDRD &= ~(1<<PD1 | 1<<PD0);
	PORTB |= 1<< PD0 | 1 << PD1; //pull-up
	
	relay_init();
	
	//Rotary Encoder Clock PIN - INT0
	//Set INT0 Sense Control - falling edge
	set_bit(MCUCR, ISC01);
	set_bit(GICR, INT0);
	
	//Configure Timer2: 1000Hz/1ms
	set_bit(TCCR2, CS21);
	set_bit(TCCR2, WGM21); //Mode 2: CTC
	OCR2 = 124;
	set_bit(TIMSK, OCIE2); // Set interrupt on compare match
	
	setup_pwm();
	
	sei();
	
    while (1) 
    {
		if(save_delay) {
			eeprom_update_byte(EEPROM_ADDR, delay);
			save_delay = 0;
		}
 		if(is_off()) {
			turnoff(); //turn off relay
		} else {
			turnon(); // turn on relay	(active low)		
		}
    }
}

