/*
 * Display.h
 *
 * Created: 6/24/2020 7:48:37 PM
 *  Author: Hassansin
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>
#include <stdbool.h>

#define DISPLAY_FIRST_DIGIT PB3
#define DISPLAY_SECOND_DIGIT PB2
#define DISPLAY_CTRL_PORT PORTB
#define DISPLAY_CTRL_DDR DDRB
#define DP 1 << 6

void display_init();
void display_enable_first_segment();
void display_enable_second_segment();
void display_enable_both_segments();
void display_disable_both_segments();
void display_toggle_both_segments();
void display_set_data(uint8_t num, bool dp);

#endif /* DISPLAY_H_ */
