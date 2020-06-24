/*
 * Buzzer.h
 *
 * Created: 6/24/2020 5:04:50 PM
 *  Author: Hassansin
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include <avr/io.h>

#define BUZZER_DDR DDRB
#define BUZZER_PORT PORTB
#define BUZZER PB1

void buzzer_enable(void);
void buzzer_disable(void);
void buzzer_toggle(void);
void buzzer_init(void);

#endif /* BUZZER_H_ */
