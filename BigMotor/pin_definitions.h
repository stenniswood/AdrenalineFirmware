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

#define LED_3 		LED_LIMIT_1
#define LED_3_DDR	DDRD
#define LED_3_PORT  PORTD
#define LED_4 		LED_LIMIT_2
#define LED_4_DDR	DDRD
#define LED_4_PORT  PORTD

#include "../core/leds.h"

#define SET_LED_LIMIT_MIN()		SET_LED_3()
#define SET_LED_LIMIT_MAX()		SET_LED_4()
#define RESET_LED_LIMIT_MIN()	RESET_LED_3()
#define RESET_LED_LIMIT_MAX()	RESET_LED_4()

// INPUTS :
#define RXCAN 				3
#define CURRENT_SENSE_LEFT 	4
#define CURRENT_SENSE_RIGHT	5
#define CURRENT_SENSE_DDR	DDRC
#define CURRENT_SENSE_PORT	PORTC
#define PORTC_INPUTS 		((1<<CURRENT_SENSE_LOW) | (1<<CURRENT_SENSE_HIGH) | (1<<RXCAN))

#define POT					5
#define POT_DDR				DDRD
#define POT_PORT			PORTD

#define ENCODER_PORT		PORTB
#define ENCODER_DDR			DDRB
#define ENCODER_Q1			3
#define ENCODER_Q2			4
#define INDEX				2
#define ENCODER_DDR_INPUT_MASK ((1<<ENCODER_Q1) | (1<<ENCODER_Q2) |	(1<<INDEX))


// OUTPUTS :
#define TXCAN 				2			// PortC
#define ENABLE_LEFT			5
#define ENABLE_RIGHT		6
#define ENABLE_DDR 		DDRB
#define ENABLE_DDR_MASK 	((1<<ENABLE_LEFT) | (1<<ENABLE_RIGHT))

#define RANDOM_NUMBER_ADC_MUX 0x43

#endif
