/*
 * Buzzer.c
 *
 * Created: 6/24/2020 5:07:04 PM
 *  Author: Hassansin
 */

#include "Buzzer.h"

// initialize buzzer io pin
void buzzer_init() {
  BUZZER_DDR |= 1 << BUZZER;
  BUZZER_PORT |= 1 << BUZZER; // off
}
void buzzer_beep(uint16_t freq, float duration) {
  float elaspsed = 0;
  while (elaspsed < duration) {
    buzzer_play(freq, 250); // play for 250ms
    _delay_ms(250);         // stop for 250ms
    elaspsed += 0.50;
  }
}
void buzzer_play(uint16_t freq, uint16_t duration) {
  float period = 1000 / (float)freq; // in millisecond
  int16_t cycles = duration / period;

  for (; cycles >= 0; cycles--) {
    BUZZER_PORT &= ~(1 << BUZZER); // on
    //_delay_us((int)(1000*period)/2);
    _delay_ms(1);
    BUZZER_PORT |= 1 << BUZZER; // off
    //_delay_us((int)(1000*period)/2);
    _delay_ms(1);
  }
}

// tone frequency and duration (in seconds) of tone]
/*
void buzzer_enable(uint16_t freq, float duration) {
  float period = 1000/freq; //in millisecond
  uint16_t cycles = duration*1000/period;
  for (;cycles>0;cycles--) {
    BUZZER_PORT &= ~(1 << BUZZER); // on
    _delay_ms(period/2);
    BUZZER_PORT |= 1 << BUZZER; // off
    _delay_ms(period/2);
  }
  BUZZER_PORT |= 1 << BUZZER; //off
}
*/
// buzzer
void buzzer_pwm_init() {
  BUZZER_DDR |= 1 << BUZZER;
  BUZZER_PORT |= 1 << BUZZER; // for active low buzzer, set the pin High
  // Configure Timer1:PWM, f = 1M/ICR1A
  BUZZER_DDR |= 1 << BUZZER;
  TCCR1A |= 1 << WGM11;              // Mode 14
  TCCR1B |= 1 << WGM12 | 1 << WGM13; // Mode 14
  ICR1 = 2000;
  OCR1A = 1000;
  TCCR1B |= 1 << CS10;
}

void buzzer_pwm_enable() {
  TCCR1A |= 1 << COM1A1 | 1 << COM1A0; // inverting
}

void buzzer_pwm_disable() {
  TCCR1A &= ~(1 << COM1A1 | 1 << COM1A0);
  BUZZER_PORT |= 1 << BUZZER;
}

void buzzer_pwm_toggle() {
  if (bit_is_set(TCCR1A, COM1A1)) {
    buzzer_pwm_disable();
    return;
  }
  buzzer_pwm_enable();
}
