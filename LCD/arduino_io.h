/*
 * arduino_io.h
 * this file maps arduino pins to avr ports and pins
 *
 * The header file: pins_arduino.h is used if this exits
 * otherwise the following controllers are defined in this file
 * Arduino (ATmega8,168,328), Mega, Sanguino (ATmega644P)
 * 
 * Thanks to Paul Stoffregen (http://www.pjrc.com/teensy) 
 * for his expertise in Arduino pin mapping macros
 *
 * Warning: when adding new board/procesor types ther are other headers
 * that have similar dependencies most notabily the ones in glcd/config
*/
#include "pins_arduino.h"



#define digitalPinToPortReg(P) \
    (    ((P) >= 0 && (P) <= 7)   ? &PORTB : \
	    (  ((P) >= 8 && (P) <= 15)  ? &PORTC : &PORTD ) \
	)
	

#define digitalPinToBit(P) \
     (((P) >= 0 && (P) <= 23) ? (P%8) : (7-(P%8)) )

