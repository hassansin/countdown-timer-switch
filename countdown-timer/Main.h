/*
 * Main.h
 *
 * Created: 6/24/2020 12:20:36 PM
 *  Author: Hassansin
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/portpins.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "GlobalDefinitions.h"
#include "pinDefines.h"
#include "Rotary.h"
#include "Buzzer.h"

#define DELAY_STEP 5 //minutes
#define MIN_DELAY 0 //minutes
#define MAX_DELAY 30 //minutes
#define SECONDS_PER_MIN 10 //seconds
#define EEPROM_ADDR ( uint8_t *) 0
#define DP 1<<7
#define WAIT_TIME 4 //seconds, time to wait after the changing counter

#define is_waiting() (wait_counter != 0)
#define is_off() (delay == 0 || is_waiting())

#define turnoff() (RELAY_PORT |= 1 << RELAY)
#define turnon() (RELAY_PORT &= ~(1 << RELAY))



#endif /* MAIN_H_ */