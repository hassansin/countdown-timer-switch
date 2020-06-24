/*
* countdown-timer.c
*
* Created: 6/10/2020 10:55:04 PM
* Author : Hassansin
*/

#include "Main.h"


volatile uint8_t finished;
volatile uint16_t ms;
volatile uint8_t wait_counter;
volatile int8_t delay; //total timer delay in minute
volatile uint8_t seconds; //counts seconds, reset every minute
volatile uint8_t save_delay;


//uint8_t digits[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90}; // common anode
uint8_t digits[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67}; // common cathode



void rot_enc_change(int8_t val) {
	wait_counter = WAIT_TIME;
	finished = 0;
	seconds = 0;
	buzzer_disable();
	delay = rotary_constrain(delay + val, MIN_DELAY, MAX_DELAY);
}

//Routine for External Interrupt 0
ISR(INT0_vect) {
	int8_t delta= rotary_detect_change();
	rot_enc_change(delta);
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
		buzzer_toggle();
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


//Invoked at 1ms interval
ISR(TIMER2_COMP_vect) {
	ms++;
	if(ms == 1000) {
		ms = 0;
	}
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
	
	rotary_init();
	relay_init();
	
	
	//Configure Timer2: 1000Hz/1ms
	set_bit(TCCR2, CS21);
	set_bit(TCCR2, WGM21); //Mode 2: CTC
	OCR2 = 124;
	set_bit(TIMSK, OCIE2); // Set interrupt on compare match
	
	buzzer_init();
	
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

