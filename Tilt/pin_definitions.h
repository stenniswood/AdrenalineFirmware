/* 
pins_arduino.h for the adrenaline_device_atmega16m1 board has been
done such that the numbers correspond 1 for 1 with the chip.  
*/
#ifndef _PIN_DEFS_
#define _PIN_DEFS_
//#include <avr/io.h>

#define BOARD_MODEL Adrenaline_Tilt

/********************* BOARD REVISION 1 ***************/
#define DELAY 100
#define STROBE_SPEED 10			// tenth of second

// TILT BOARD LEDS:
#define LED_1_2_USEABLE		1
#define LED_1 	0
#define LED_2 	1
#define LED_3 	6
#define LED_4 	7
#define LED_DDR_MASK 	((1<<LED_1) | (1<<LED_2) | (1<<LED_3) | (1<<LED_4))
#define LED_1_DDR 		DDRD
#define LED_2_DDR 		DDRD
#define LED_3_DDR 		DDRD
#define LED_4_DDR 		DDRD
#define LED_1_PORT 		PORTD
#define LED_2_PORT 		PORTD
#define LED_3_PORT 		PORTD
#define LED_4_PORT 		PORTD


// OUTPUTS 
#define CS_ACCEL  	(1<<2)
#define CS_GYRO   	(1<<3)
#define CS_MAGNET 	(1<<4)
#define CS_DDR_MASK ((CS_ACCEL) | (CS_GYRO) | (CS_MAGNET))
#define CS_DDR		DDRD
#define CS_PORT		PORTD

//#define USES_ADC 
#define RANDOM_NUMBER_ADC_MUX 0x43		// most use LED3
#define RANDOM_NUMBER_PIN_INPUT()   { LED_3_DDR &=(~LED_3); };
#define RANDOM_NUMBER_PIN_OUTPUT()  { LED_3_DDR |=LED_3;    };


#define SET_CS_ACCEL_LOW   { CS_PORT &= (~CS_ACCEL);	}	
#define SET_CS_ACCEL_HIGH  { CS_PORT |=  CS_ACCEL;	}	
#define SET_CS_GYRO_LOW    { CS_PORT &= ~CS_GYRO;	}	
#define SET_CS_GYRO_HIGH   { CS_PORT |=  CS_GYRO;	}
#define SET_CS_MAGNET_LOW  { CS_PORT &= ~CS_MAGNET;	}
#define SET_CS_MAGNET_HIGH { CS_PORT |=  CS_MAGNET;	}


// INPUTS:
#define GYRO_INT1  (1<<1)
#define ACCEL_INT1 (1<<6)
#define ACCEL_INT2 (1<<7)
#define INT_DDR    DDRC
#define INT_PORT   PORTC
#define INT_PORT_IN   PINC
#define INT_DDR_MASK (GYRO_INT1 | ACCEL_INT1 | ACCEL_INT2)

// INPUTS:
#define BUTTON1 (1<<3)
#define BUTTON2 (1<<4)
#define BUTTON_DDR_MASK (BUTTON1 | BUTTON2)
#define BUTTON_DDR DDRB 
#define BUTTON_PORT PINB

#define DRDY 30


#endif
