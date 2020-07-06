/*
 * Main.h
 *
 * Created: 6/24/2020 12:20:36 PM
 *  Author: Hassansin
 */

#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 1000000UL

#include "Buzzer.h"
#include "Display.h"
#include "Relay.h"
#include "Rotary.h"
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/portpins.h>
#include <stdbool.h>
#include <util/delay.h>

#define DELAY_STEP 5       // minutes
#define MIN_DELAY 0        // minutes
#define MAX_DELAY 30       // minutes
#define SECONDS_PER_MIN 60 // seconds
#define EEPROM_ADDR (uint8_t *)0
#define WAIT_TIME 4     // seconds, time to wait after the changing counter
#define BEEP_DURATION 3 // seconds
#define BEEP_FREQ 1000
#define CALIBRATED_MS_PER_SECOND 980

#define is_waiting() (wait_counter != 0)
#define is_off() (delay == 0 || is_waiting())

#define rotary_constrain(v, min, max)                                          \
  ((v) < (min) ? (max) : (v) > (max) ? (min) : (v))
#define constrain(v, min, max) ((v) < (min) ? (min) : (v) > (max) ? (max) : (v))
#define _NOP()                                                                 \
  do {                                                                         \
    __asm__ __volatile__("nop");                                               \
  } while (0)

#endif /* MAIN_H_ */
