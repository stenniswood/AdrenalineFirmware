/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

#define NUM_DIGITAL_PINS            32
#define NUM_ANALOG_INPUTS           9
#define analogInputToDigitalPin(p)  ((p < 16) ? (p) + 54 : -1)
#define digitalPinHasPWM(p)         (((p) >= 2 && (p) <= 13) || ((p) >= 44 && (p)<= 46))


static const uint8_t SS   = 3;
static const uint8_t MOSI = 2;
static const uint8_t MISO = 1;
static const uint8_t SCK  = 12;

static const uint8_t SDA = 20;
static const uint8_t SCL = 21;
static const uint8_t LED1_BUILTIN = 13;
static const uint8_t LED2_BUILTIN = 14;
static const uint8_t LED3_BUILTIN = 15;

static const uint8_t A0 = 28;
static const uint8_t A1 = 13;
static const uint8_t A2 = 14;
static const uint8_t A3 = 16;
static const uint8_t A4 = 17;
static const uint8_t A5 = 18;
static const uint8_t A6 = 22;
static const uint8_t A7 = 26;
static const uint8_t A8 = 27;
static const uint8_t A9 = 28;

// A majority of the pins are NOT PCINTs, SO BE WARNED (i.e. you cannot use them as receive pins)
// Only pins available for RECEIVE (TRANSMIT can be on any pin):
// (I've deliberately left out pin mapping to the Hardware USARTs - seems senseless to me)
// Pins: 10, 11, 12, 13,  50, 51, 52, 53,  62, 63, 64, 65, 66, 67, 68, 69

#define digitalPinToPCICR(p)    ( (((p) >= 10) && ((p) <= 13)) || \
                                  (((p) >= 50) && ((p) <= 53)) || \
                                  (((p) >= 62) && ((p) <= 69)) ? (&PCICR) : ((uint8_t *)0) )

#define digitalPinToPCICRbit(p) ( (((p) >= 10) && ((p) <= 13)) || (((p) >= 50) && ((p) <= 53)) ? 0 : \
                                ( (((p) >= 62) && ((p) <= 69)) ? 2 : \
                                0 ) )

#define digitalPinToPCMSK(p)    ( (((p) >= 10) && ((p) <= 13)) || (((p) >= 50) && ((p) <= 53)) ? (&PCMSK0) : \
                                ( (((p) >= 62) && ((p) <= 69)) ? (&PCMSK2) : \
                                ((uint8_t *)0) ) )

#define digitalPinToPCMSKbit(p) ( (((p) >= 10) && ((p) <= 13)) ? ((p) - 6) : \
                                ( ((p) == 50) ? 3 : \
                                ( ((p) == 51) ? 2 : \
                                ( ((p) == 52) ? 1 : \
                                ( ((p) == 53) ? 0 : \
                                ( (((p) >= 62) && ((p) <= 69)) ? ((p) - 62) : \
                                0 ) ) ) ) ) )

#ifdef ARDUINO_MAIN

const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
	(uint16_t) &DDRE
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
	(uint16_t) &PORTE
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PIN,
	NOT_A_PORT,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
	(uint16_t) &PINE
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	// PORTLIST		
	// -------------------------------------------		
	PD	, // N/A  ** 0 ** UNUSED  
	PD	, // PD 2 ** 1 ** MISO_A
	PD	, // PD 3 ** 2 ** MOSI_A
	PC	, // PC 1 ** 3 ** SS_A	
	PA	, // VCC  ** 4 ** VCC	
	PA	, // GND  ** 5 ** GND	
	PC	, // PC 2 ** 6 ** TXCAN	
	PC	, // PC 3 ** 7 ** RXCAN	
	PB	, // PB 0 ** 8 ** PWM7	
	PB	, // PB 1 ** 9 ** PWM8	
	PE	, // PE 1 ** 10 ** XTAL1
	PE	, // PE 2 ** 11 ** XTAL2	
	PD	, // PD 4 ** 12 ** SCK_A	
	PD	, // PD 5 ** 13 ** A1	
	PD	, // PD 6 ** 14 ** A2	
	PD	, // PD 7 ** 15 **  	
	PB	, // PB 2 ** 16 ** A3	
	PC	, // PC 4 ** 17 ** A4	
	PC	, // PC 5 ** 18 ** A5	
	PA	, // VCC  ** 19 * VCC	
	PA	, // GND  ** 20 **  GND	
	PA	, // AREF ** 21 ** AREF	
	PC	, // PC 6 ** 22 ** A6	
	PB	, // PB 3 ** 23 ** 	
	PB	, // PB 4 ** 24 ** 	
	PC	, // PC 7 ** 25 ** 
	PB	, // PB 5 ** 26 ** A7	
	PB	, // PB 6 ** 27 ** A8	
	PB	, // PB 7 ** 28 ** A9	
	PD	, // PD 0 ** 29 ** PWM	
	PC	, // PC 0 ** 30 ** D29	
	PE	, // PE 0 ** 31 ** RESET	
	PD	  // PD 1 ** 32 ** D31	
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	// PIN IN PORT		
	// -------------------------------------------		
	_BV( 0 )	, // PD 0 **  0 **  UNUSED
	_BV( 2 )	, // PD 2 **  1 ** MISO_A
	_BV( 3 )	, // PD 3 **  2 ** MOSI_A
	_BV( 1 )	, // PC 1 **  3 ** SS_A	
	_BV( 0 )	, // VCC  **  4 ** VCC	
	_BV( 0 )	, // GND  **  5 ** GND	
	_BV( 2 )	, // PC 2 **  6 ** TXCAN	
	_BV( 3 )	, // PC 3 **  7 ** RXCAN	
	_BV( 0 )	, // PB 0 **  8 ** PWM7	
	_BV( 1 )	, // PB 1 **  9 ** PWM8	
	_BV( 1 )	, // PE 1 ** 10 ** XTAL1
	_BV( 2 )	, // PE 2 ** 11 ** XTAL2	
	_BV( 4 )	, // PD 4 ** 12 ** SCK_A	
	_BV( 5 )	, // PD 5 ** 13 ** A1	
	_BV( 6 )	, // PD 6 ** 14 ** A2	
	_BV( 7 )	, // PD 7 ** 15 **  	
	_BV( 2 )	, // PB 2 ** 16 ** A3	
	_BV( 4 )	, // PC 4 ** 17 ** A4	
	_BV( 5 )	, // PC 5 ** 18 ** A5	
	_BV( 0 )	, // VCC  ** 19 ** VCC	
	_BV( 0 )	, // GND  ** 20 ** GND	
	_BV( 0 )	, // AREF ** 21 ** AREF	
	_BV( 6 )	, // PC 6 ** 22 ** A6	
	_BV( 3 )	, // PB 3 ** 23 ** 	
	_BV( 4 )	, // PB 4 ** 24 ** 	
	_BV( 7 )	, // PC 7 ** 25 ** 
	_BV( 5 )	, // PB 5 ** 26 ** A7	
	_BV( 6 )	, // PB 6 ** 27 ** A8	
	_BV( 7 )	, // PB 7 ** 28 ** A9	
	_BV( 0 )	, // PD 0 ** 29 ** PWM	
	_BV( 0 )	, // PC 0 ** 30 ** D29	
	_BV( 0 )	, // PE 0 ** 31 ** RESET	
	_BV( 1 )	  // PD 1 ** 32 ** D31	
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	// TIMERS		
	// -------------------------------------------		
	TIMER1A	, // PE 0 ** 0 ** USART0_RX	
	TIMER0A	, // PE 1 ** 1 ** USART0_TX	
	TIMER1B	, 		// PE 4 ** 2 ** PWM2	
	NOT_ON_TIMER	, 		// PE 5 ** 3 ** PWM3	
	NOT_ON_TIMER	, 		// PG 5 ** 4 ** PWM4	
	NOT_ON_TIMER	, 		// PE 3 ** 5 ** PWM5	
	NOT_ON_TIMER	, 		// PH 3 ** 6 ** PWM6	
	NOT_ON_TIMER	, 		// PH 4 ** 7 ** PWM7	
	NOT_ON_TIMER	, 		// PH 5 ** 8 ** PWM8	
	NOT_ON_TIMER	, 		// PH 6 ** 9 ** PWM9	
	NOT_ON_TIMER	, 		// PB 4 ** 10 ** PWM10	
	NOT_ON_TIMER	, 		// PB 5 ** 11 ** PWM11	
	NOT_ON_TIMER	, 		// PB 6 ** 12 ** PWM12	
	NOT_ON_TIMER	, 		// PB 7 ** 13 ** PWM13	
	NOT_ON_TIMER	, // PJ 1 ** 14 ** USART3_TX	
	NOT_ON_TIMER	, // PJ 0 ** 15 ** USART3_RX	
	NOT_ON_TIMER	, // PH 1 ** 16 ** USART2_TX	
	NOT_ON_TIMER	, // PH 0 ** 17 ** USART2_RX	
	NOT_ON_TIMER	, // PD 3 ** 18 ** USART1_TX	
	NOT_ON_TIMER	, // PD 2 ** 19 ** USART1_RX	
	NOT_ON_TIMER	, // PD 1 ** 20 ** I2C_SDA	
	NOT_ON_TIMER	, // PD 0 ** 21 ** I2C_SCL	
	NOT_ON_TIMER	, // PA 0 ** 22 ** D22	
	NOT_ON_TIMER	, // PA 1 ** 23 ** D23	
	NOT_ON_TIMER	, // PA 2 ** 24 ** D24	
	NOT_ON_TIMER	, // PA 3 ** 25 ** D25	
	NOT_ON_TIMER	, // PA 4 ** 26 ** D26	
	NOT_ON_TIMER	, // PA 5 ** 27 ** D27	
	NOT_ON_TIMER	, // PA 6 ** 28 ** D28	
	NOT_ON_TIMER	, // PA 7 ** 29 ** D29	
	NOT_ON_TIMER	, // PC 7 ** 30 ** D30	
	NOT_ON_TIMER	  // PC 6 ** 31 ** D31	
};

#endif

#endif