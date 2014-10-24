/* pins_arduino.h for the adrenaline_device_atmega16m1 board has been
done such that the numbers correspond 1 for 1 with the chip.  
*/
#ifndef _PIN_DEFS_
#define _PIN_DEFS_



/********************* BOARD REVISION 1 ***************/
#define DELAY 100

// BUTTON BOARD LEDS :
#define LED_1_2_USEABLE	1
#define LED_1 			0
#define LED_2 			1
#define LED_3 			6
#define LED_4 			7
#define LED_DDR_MASK 	((1<<LED_1) | (1<<LED_2) | (1<<LED_3) | (1<<LED_4))

#define LED_1_DDR 		DDRD
#define LED_2_DDR 		DDRD
#define LED_3_DDR 		DDRD
#define LED_4_DDR 		DDRD
#define LED_1_PORT 		PORTD
#define LED_2_PORT 		PORTD
#define LED_3_PORT 		PORTD
#define LED_4_PORT 		PORTD

#define BOARD_MODEL 	Adrenaline_Button

#define MISO (1<<0)
#define MOSI (1<<1)
#define SCLK (1<<7)

// INPUTS:
#define RANDOM_NUMBER_ADC_MUX 0x43		// most use LED3
#define RANDOM_NUMBER_PIN_INPUT()   { LED_3_DDR &=(~LED_3); };
#define RANDOM_NUMBER_PIN_OUTPUT()  { LED_3_DDR |=LED_3;    };

/********************* REVISION r2 ***************
#define LED1 29
#define LED2 32
#define LED3 14
#define LED4 15

#define CS_ACCEL 8
#define CS_GYRO  9
#define CS_MAGNET 16

#define BUTTON1 23
#define BUTTON2 24

#define GYRO_INT1   3
#define ACCEL_INT1 22
#define ACCEL_INT2 25

#define DRDY 

#define A3 
#define A4 18
#define A5 13
#define A6 26
#define A7 27
#define A8 17
****************************************/
#endif
