/*
 * Relay.h
 *
 * Created: 6/24/2020 7:50:11 PM
 *  Author: Hassansin
 */

#ifndef RELAY_H_
#define RELAY_H_

#define RELAY_PORT PORTD
#define RELAY_DDR DDRD
#define RELAY PD0

void relay_init(void);
void relay_on(void);
void relay_off(void);

#endif /* RELAY_H_ */
