/*
 * countdown-timer.c
 *
 * Created: 6/10/2020 10:55:04 PM
 * Author : Hassansin
 */

#include "Main.h"

volatile uint16_t ms;
volatile uint8_t wait_counter;
volatile int8_t delay;    // total timer delay in minute
volatile uint8_t seconds; // counts seconds, reset every minute
volatile bool save_delay;
volatile bool finished;
volatile uint8_t beep_elapsed; //beep_duration in seconds

void rot_enc_change(int8_t val) {
  wait_counter = WAIT_TIME;
  finished = false;  
  seconds = 0;
  buzzer_pwm_disable();
  delay = rotary_constrain(delay + val, MIN_DELAY, MAX_DELAY);
}

void update_display(uint16_t cs) {
  uint8_t d1 = delay % 10;
  uint8_t d2 = delay / 10 % 10;

  // 1 centi second = 10ms
  // Toggle segments at every 10ms
  // Show the dot point only at the beginning of a second AND when countdown
  // timer is on
  if (cs == 0 && !is_off()) {
    display_enable_first_segment();
    display_set_data(d1, true);
  } else if (cs % 2 == 0) {
    display_enable_first_segment();
    display_set_data(d1, false);
  } else if (d2 != 0) { // disable second segment if value is zero
    display_enable_second_segment();
    display_set_data(d2, false);
  }
}

void flash_display(uint16_t cs) {
  display_set_data(0, false);
  if (cs % 25 == 0) {
    display_toggle_both_segments();
  }
}

void tone(uint8_t cs) {
  if(beep_elapsed >= BEEP_DURATION) {
    buzzer_pwm_disable();
    return;
  }
  if(cs == 0 ) {
    beep_elapsed++;
  }  
  if (cs % 25 == 0) {
    buzzer_pwm_toggle();
  }
}

void delay_countdown(uint16_t sec) {
  if (is_off()) {
    return;
  }
  seconds++;
  if (seconds == SECONDS_PER_MIN) {
    seconds = 0;
    delay = rotary_constrain(delay - 1, MIN_DELAY, MAX_DELAY);
  }
  if (delay == 0) {
    finished = true;
    beep_elapsed = 0;
  }
}

void wait_countdown(uint16_t sec) {
  if (!is_waiting()) {
    return;
  }
  wait_counter--;
  if (wait_counter == 0) {
    save_delay = true;
  }
}

void at_each_centi_second(uint8_t cs) {
  if (finished) {
    flash_display(cs);
    tone(cs);
  } else {
    update_display(cs);
  }
}

void at_each_second(uint8_t sec) {
  delay_countdown(sec);
  wait_countdown(sec);
}

// Invoked at 1ms interval
ISR(TIMER2_COMP_vect) {
  ms++;
  if (ms == 1000) {
    ms = 0;
  }
  if (ms % 10 == 0) {
    at_each_centi_second(ms / 10);
  }
  if (ms == 0) {
    at_each_second(ms);
  }
}

int main(void) {
  cli();
  // Read from EEROM, First read might return garbage value
  delay = rotary_constrain(eeprom_read_byte(EEPROM_ADDR), MIN_DELAY, MAX_DELAY);
  //delay = 1;
  wait_counter = 2 * WAIT_TIME;

  display_init();
  rotary_init(&rot_enc_change);
  relay_init();
  buzzer_pwm_init();

  // Configure Timer2: 1000Hz/1ms
  TCCR2 |= 1<< CS21;
  TCCR2 |= 1<< WGM21; // Mode 2: CTC
  OCR2 = 124;
  TIMSK |= 1<<OCIE2; // Set interrupt on compare match

  sei();

  while (1) {
    if (save_delay) {
      eeprom_update_byte(EEPROM_ADDR, delay);
      save_delay = false;
    }
    if (is_off()) {
      relay_off();
    } else {
      relay_on();
    }    
  }
}
