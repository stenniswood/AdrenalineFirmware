/* pins_arduino.h for the adrenaline_device_atmega16m1 board has been
done such that the numbers correspond 1 for 1 with the chip.  
*/
#ifndef _PIN_DEFS_
#define _PIN_DEFS_


#define BOARD_MODEL Adrenaline_BigMotor
/*************** BIG MOTOR BOARD REVISION D QUAD ***************/
#define DELAY 100


///////////////////////////////////////
// MOTOR DEFINITIONS : 
///////////////////////////////////////
#define MOTOR_PWM_DDR  DDRB
#define MOTOR_PWM_PORT PORTB
#define PWM_MOT_1 		7
#define PWM_MOT_2 		6
#define PWM_MOT_3 		5
#define PWM_MOT_4 		4




///////////////////////////////////////
#define LIMIT_MIN 	3
#define LIMIT_MAX 	4

//#include "../core/leds.h"

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


#define INDEX				2
#define ENCODER_DDR_INPUT_MASK ((1<<ENCODER_Q1) | (1<<ENCODER_Q2) |	(1<<INDEX))

// OUTPUTS:
#define TXCAN 				2			// PortC
/* REV C PATCH SWITCHes the DIR and PWM pins.  So now HILOW_LEFT & HILOW_RIGHT,
	operate the inhibit signal.
	INHIBIT is ACTIVE LOW, so this would be better called ENABLE_LEFT & ENABLE_RIGHT.
	
*/
#define INHIBIT_LEFT			5
#define INHIBIT_RIGHT			6
#define INHIBIT_DDR 			DDRB
#define INHIBIT_PORT 			PORTB
#define INHIBIT_DDR_MASK 		((1<<INHIBIT_LEFT) | (1<<INHIBIT_RIGHT))

#define RANDOM_NUMBER_ADC_MUX 0x43

#endif
