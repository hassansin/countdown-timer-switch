/*
 * GlobalDefinitions.h
 *
 * Created: 6/24/2020 12:18:58 PM
 *  Author: Hassansin
 */

#ifndef GLOBALDEFINITIONS_H_
#define GLOBALDEFINITIONS_H_

#include <avr/sfr_defs.h>

#define set_bit(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define clear_bit(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))

#endif /* GLOBALDEFINITIONS_H_ */
