		/*****************************
				Analog Board
		******************************/
/* pins_arduino.h for the adrenaline_device_atmega16m1 board has been
done such that the numbers correspond 1 for 1 with the chip.  
*/
#ifndef _PIN_DEFS_
#define _PIN_DEFS_

#define byte unsigned char
#define word unsigned short

/********************* BOARD REVISION 1 ***************/
#define DELAY 100

#include "can_id_list.h"

//#define USES_ADC 
#define RANDOM_NUMBER_ADC_MUX   0x43		// most use LED3
#define RANDOM_NUMBER_PIN_INPUT ()   { LED_3_DDR &=(~LED_3); };
#define RANDOM_NUMBER_PIN_OUTPUT()   { LED_3_DDR |=LED_3;    };
#define BOARD_MODEL   Adrenaline_Analog


// Analog BOARD LEDS:
#define LED_1_2_USEABLE		1
#define LED_1 6			// switched b/c board layout.
#define LED_2 5			// switched b/c board layout.
#define LED_3 7
#define LED_4 1
#define LED_DDR_MASK 	((1<<LED_1) | (1<<LED_2) | (1<<LED_3) | (1<<LED_4))

#define LED_1_DDR 		DDRD
#define LED_2_DDR 		DDRD
#define LED_3_DDR 		DDRD
#define LED_4_DDR 		DDRD
#define LED_1_PORT 		PORTD
#define LED_2_PORT 		PORTD
#define LED_3_PORT 		PORTD
#define LED_4_PORT 		PORTD

// OUTPUTS :
#define CS_A1	3
#define CS_A2	4
#define CS_A3	5
#define CS_A4	6
#define CS_DDR_MASK ((1<<CS_A1) | (1<<CS_A2) | (1<<CS_A3)| (1<<CS_A4))
#define CS_DDR  DDRB
#define CS_PORT PORTB


// END OF CONVERSIONS : 
#define _EOC1 4
#define _EOC2 5
#define _EOC3 6
#define _EOC4 7
#define EOC_DDR_MASK ((1<<_EOC1) | (1<<_EOC2) | (1<<_EOC3)| (1<<_EOC4))
#define EOC_DDR  DDRC
#define EOC_PORT PORTC
#define EOC_IN   PINC


// INPUTS:
#define DRDY 30

#define A3 14
#define A4 18
#define A5 16
#define A6 26
#define A7 27
#define A8 17

/********************* REVISION r2 ***************
****************************************/
#endif
