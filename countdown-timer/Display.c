/*
 * Display.c
 *
 * Created: 6/24/2020 8:00:12 PM
 *  Author: Hassansin
 */

#include "Display.h"

// uint8_t digits[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90}; //
// common anode
uint8_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
                    0x6D, 0x7D, 0x07, 0x7F, 0x67}; // common cathode

void display_init() {
  // Display PORT
  DDRB |= 0b11111000;
  DDRD |= 0b11100000;

  // Display Control
  DDRC |= 1 << PC0 | 1 << PC1;
  PORTC |= 1 << PC1; // turn off second display
}

void display_enable_first_segment() {
  PORTC |= 1 << PC1;
  PORTC &= ~(1 << PC0);
}

void display_enable_second_segment() {
  PORTC |= 1 << PC0;
  PORTC &= ~(1 << PC1);
}

void display_enable_both_segments() { PORTC &= ~(1 << PC0 | 1 << PC1); }

void display_disable_both_segments() { PORTC |= 1 << PC0 | 1 << PC1; }

void display_toggle_both_segments() {
  if (bit_is_set(PORTC, PC0) || bit_is_set(PORTC, PC1)) {
    display_enable_both_segments();
  } else {
    display_disable_both_segments();
  }
}

void display_set_data(uint8_t num, bool dp) {
  uint8_t data = digits[num];
  if (dp) {
    data |= DP;
  }
  uint8_t temp = PORTB;
  temp &= ~0b11111000;
  temp |= (data & 0b11111000);
  PORTB = temp;

  temp = PORTD;
  temp &= ~0b11100000;
  temp |= (data & 0b00000111) << 5;
  PORTD = temp;
}
