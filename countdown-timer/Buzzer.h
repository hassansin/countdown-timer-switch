/*
 * Buzzer.h
 *
 * Created: 6/24/2020 5:04:50 PM
 *  Author: Hassansin
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#define F_CPU 1000000UL

#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#define BUZZER_DDR DDRB
#define BUZZER_PORT PORTB
#define BUZZER PB1

void buzzer_play(uint16_t freq, uint16_t duration);
void buzzer_beep(uint16_t freq, float duration);
void buzzer_init(void);
void buzzer_pwm_init();
void buzzer_pwm_enable();
void buzzer_pwm_disable();
void buzzer_pwm_toggle();

#endif /* BUZZER_H_ */
