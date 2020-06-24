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
#define rotary_constrain(v, min, max)                                          \
  ((v) < (min) ? (max) : (v) > (max) ? (min) : (v))
#define constrain(v, min, max) ((v) < (min) ? (min) : (v) > (max) ? (max) : (v))

#endif /* GLOBALDEFINITIONS_H_ */
