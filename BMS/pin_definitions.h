/* pins_arduino.h for the adrenaline_device_atmega16m1 board has been
done such that the numbers correspond 1 for 1 with the chip.  
*/
#ifndef _PIN_DEFS_
#define _PIN_DEFS_

#include "../core/bk_system_defs.h"

#define BOARD_MODEL Adrenaline_BigMotor
/*************** BIG MOTOR BOARD REVISION 1 ***************/
#define DELAY 100

#define PWM_LEFT 		2
#define PWM_LEFT_DDR  DDRD
#define PWM_LEFT_PORT PORTD

#define PWM_RIGHT 		1
#define PWM_RIGHT_DDR  DDRC
#define PWM_RIGHT_PORT PORTC

// These 2 LEDs are directly lit by their control signals.
// ie. The PWM signals.
//#define LED_1 PWM_LEFT
//#define LED_2 PWM_RIGHT
#define LED_LIMIT_1 6
#define LED_LIMIT_2 7

///////////////////////////////////////
// THESE DRIVEN BY PWM IN DEVICE MODE!
// CANNOT USE LED 1&2 FOR BIG MOTOR EN BOARDS.  THEY DIRECTLY GO TO THE motor
// ENABLE LINES.  SO TURNING ON LEDS IN "MOTOR_OFF" WILL START MOVING THEM!
// THEREFORE THE MYINSTANCE WILL ONLY SHOW UPPER 2 BITS ON THESE BOARDS!



#define LED_1_2_USEABLE	(1==1) 
///////////////////////////////////////

#define LED_1		3
#define LED_1_DDR	DDRB
#define LED_1_PORT  PORTB

#define LED_2		2
#define LED_2_DDR	DDRB
#define LED_2_PORT  PORTB

#define LED_3 		1
#define LED_3_DDR	DDRB
#define LED_3_PORT  PORTB

#define LED_4 		0
#define LED_4_DDR	DDRB
#define LED_4_PORT  PORTB


#define LIMIT_MIN 	3
#define LIMIT_MAX 	4



#define LED_1_ON		RESET_LED_1
#define LED_2_ON		RESET_LED_2
#define LED_3_ON()		RESET_LED_3()
#define LED_4_ON()		RESET_LED_4()

#define LED_1_OFF()		SET_LED_1()
#define LED_2_OFF()		SET_LED_2()
#define LED_3_OFF()		SET_LED_3()
#define LED_4_OFF()		SET_LED_4()

#include "../core/leds.h"



#define SET_LED_LIMIT_MIN()		SET_LED_3()
#define SET_LED_LIMIT_MAX()		SET_LED_4()
#define RESET_LED_LIMIT_MIN()	RESET_LED_3()
#define RESET_LED_LIMIT_MAX()	RESET_LED_4()

// INPUTS :
#define RXCAN 				3
#define CURRENT_SENSE_1 	4
#define CURRENT_SENSE_2		5
#define CURRENT_SENSE_3		6
#define CURRENT_SENSE_4		7
#define CURRENT_SENSE_DDR	DDRC
#define CURRENT_SENSE_PORT	PORTC
#define PORTC_INPUTS 		((1<<CURRENT_SENSE_1) | (1<<CURRENT_SENSE_2) | (1<<CURRENT_SENSE_3) | (1<<CURRENT_SENSE_4) | (1<<RXCAN))


// OUTPUTS:
#define TXCAN 				2
#define TXCAN_DDR 			DDRC
#define TXCAN_PORT 			PORTC
#define TXCAN_DDR_MASK 		(1<<TXCAN)


#define RANDOM_NUMBER_ADC_MUX 0x43

#endif
