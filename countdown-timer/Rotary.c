/*
 * Rotary.c
 *
 * Created: 6/24/2020 11:40:50 AM
 *  Author: Hassansin
 */

#include "Rotary.h"

volatile int8_t a0 = OFF;
volatile int8_t c0 = OFF;
void (*callback)(int8_t);

int8_t rotary_detect_change(void);

// Routine for External Interrupt 0
ISR(INT0_vect) {
  int8_t delta = rotary_detect_change();
  (*callback)(delta);
}

// initializes rotary encoder interrupt
void rotary_init(void (*cb)(int8_t)) {
  ROTARY_DDR &= ~(1 << ROTARY_DATA |
                  1 << ROTARY_CLK); // Set direction of clock pin and data pin

  // TODO: comment out following when simulating in Proteus
  // ROTARY_PORT |= 1<< ROTARY_DATA | 1 << ROTARY_CLK; //enable pull-ups

  MCUCR |= 1 << ISC00; // Any logical change on INT0
  GICR |= 1 << INT0;   // enable external interrupt
  callback = cb;
}

// returns 0 or 1 or -1
int8_t rotary_detect_change() {
  int8_t a = (ROTARY_PIN >> ROTARY_CLK) & 1;
  int8_t b = (ROTARY_PIN >> ROTARY_DATA) & 1;
  if (a != a0) {
    a0 = a;
    if (b != c0) {
      if (c0 == OFF) {
        c0 = b;
        return 0;
      }
      c0 = b;
      if (a == b) {
        return 1;
      }
      return -1;
    }
  }
  return 0;
}
