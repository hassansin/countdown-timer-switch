/*
 * Display.c
 *
 * Created: 6/24/2020 8:00:12 PM
 *  Author: Hassansin
 */

#include "Display.h"

// uint8_t digits[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90}; //
// common anode
uint8_t digits[] = {0x3F, 0x18, 0xB6, 0xBC, 0x99,
                    0xAD, 0xAF, 0x38, 0xBF, 0xBD}; // common cathode

void display_init() {
  // Display PORT
  DDRC |= 0b00111111;
  DDRB |= 0b00110000;

  // Display Control
  DISPLAY_CTRL_DDR |= 1 << DISPLAY_FIRST_DIGIT | 1 << DISPLAY_SECOND_DIGIT;
  DISPLAY_CTRL_PORT |= 1 << DISPLAY_SECOND_DIGIT; // turn off second display
}

void display_enable_first_segment() {
  DISPLAY_CTRL_PORT |= 1 << DISPLAY_SECOND_DIGIT;
  DISPLAY_CTRL_PORT &= ~(1 << DISPLAY_FIRST_DIGIT);
}

void display_enable_second_segment() {
  DISPLAY_CTRL_PORT |= 1 << DISPLAY_FIRST_DIGIT;
  DISPLAY_CTRL_PORT &= ~(1 << DISPLAY_SECOND_DIGIT);
}

void display_enable_both_segments() {
  DISPLAY_CTRL_PORT &= ~(1 << DISPLAY_FIRST_DIGIT | 1 << DISPLAY_SECOND_DIGIT);
}

void display_disable_both_segments() {
  DISPLAY_CTRL_PORT |= 1 << DISPLAY_FIRST_DIGIT | 1 << DISPLAY_SECOND_DIGIT;
}

void display_toggle_both_segments() {
  if (bit_is_set(DISPLAY_CTRL_PORT, DISPLAY_FIRST_DIGIT) ||
      bit_is_set(DISPLAY_CTRL_PORT, DISPLAY_SECOND_DIGIT)) {
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
  uint8_t temp = PORTC;
  temp &= ~0b00111111;
  temp |= (data & 0b00111111);
  PORTC = temp;

  temp = PORTB;
  temp &= ~0b00110000;
  temp |= (data & 0b11000000) >> 2;
  PORTB = temp;
}
