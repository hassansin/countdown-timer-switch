/*
 * Relay.c
 *
 * Created: 6/24/2020 7:52:50 PM
 *  Author: Hassansin
 */

#include "Relay.h"
#include <avr/io.h>

// initialize relay control pin
void relay_init() {
  RELAY_DDR |= 1 << RELAY;
  relay_off();
}

void relay_off() { RELAY_PORT |= 1 << RELAY; }

void relay_on() { RELAY_PORT &= ~(1 << RELAY); }
