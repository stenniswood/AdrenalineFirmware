		/*****************************
				PWM Board
		******************************/
/* pins_arduino.h for the adrenaline_device_atmega16m1 board has been
done such that the numbers correspond 1 for 1 with the chip.  */
#ifndef _PIN_DEFS_
#define _PIN_DEFS_

/* Following moved to "bk_system_defs.h"
#define byte unsigned char  */
#include "bk_system_defs.h"

// Include "can_id_list.h" first
#include "can_id_list.h"
#define BOARD_MODEL  Adrenaline_QuadCopter

/********************* BOARD REVISION 1 ***************/
#define DELAY 100
#define STROBE_SPEED 10

// PWM BOARD LEDS:
#define LED_DDR 		DDRD
#define LED_PORT 		PORTD

#define LED_1_2_USEABLE		1
#define LED_1 6
#define LED_2 7
#define LED_3 0
#define LED_4 1
#define LED_DDR_MASK 	((1<<LED_1) | (1<<LED_2) | (1<<LED_3) | (1<<LED_4))
#define LED_1_PORT	PORTD
#define LED_2_PORT	PORTD
#define LED_3_PORT	PORTD
#define LED_4_PORT	PORTD

#define LED_1_DDR	DDRD
#define LED_2_DDR	DDRD
#define LED_3_DDR	DDRD
#define LED_4_DDR	DDRD

// OUTPUTS : 
#define LDI				2
//#define ENABLE_LOAD		3		// latches expensive - not used.
//#define ENABLE_CLEAR	4		// latches expensive - not used.
#define OPEN_LED		5
//#define EN_ACTIVE		6

//#define CONTROL_OUTPUTS ( (1<<MOSI) | (1<<LDI) | (1<<ENABLE_LOAD) | (1<<ENABLE_CLEAR) | (1<<OPEN_LED) | (1<<EN_ACTIVE) | (1<<SCLK))
#define CONTROL_OUTPUTS ( (1<<LDI) | (1<<OPEN_LED) )
#define CONTROL_PORT	PORTB
#define CONTROL_DDR		DDRB

#define SerialNumber 0x12345678


#define CS_8500 	(1<<LDI)
#define CS_DDR_MASK (CS_8500)
#define CS_DDR  	DDRB
#define CS_PORT 	PORTB


#define PWMCLK			1
#define TXCAN			2
#define RXCAN			3
#define PORTB_OUTPUTS ((1<<PWMCLK) | (1<<TXCAN))


#endif
