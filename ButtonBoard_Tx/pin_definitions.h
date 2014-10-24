/* pins_arduino.h for the adrenaline_device_atmega16m1 board has been
done such that the numbers correspond 1 for 1 with the chip.  
*/
#ifndef _PIN_DEFS_
#define _PIN_DEFS_
//#include <avr/io.h>

#define byte unsigned char
#define word unsigned short

/********************* BOARD REVISION 1 ***************/
#define DELAY 100

// BUTTON BOARD LEDS:
#define LED_1 0
#define LED_2 1
#define LED_3 6
#define LED_4 7
#define LED_DDR_MASK 	((1<<LED_1) | (1<<LED_2) | (1<<LED_3) | (1<<LED_4))
#define LED_DDR 		DDRD
#define LED_PORT 		PORTD

#define SET_LED_1 	LED_PORT |= (1<<LED_1);
#define SET_LED_2 	LED_PORT |= (1<<LED_2);
#define SET_LED_3 	LED_PORT |= (1<<LED_3);
#define SET_LED_4 	LED_PORT |= (1<<LED_4);

#define RESET_LED_1 LED_PORT &= ~(1<<LED_1);
#define RESET_LED_2 LED_PORT &= ~(1<<LED_2);
#define RESET_LED_3 LED_PORT &= ~(1<<LED_3);
#define RESET_LED_4 LED_PORT &= ~(1<<LED_4);





#define MISO (1<<0)
#define MOSI (1<<1)
#define SCLK (1<<7)

#define lo_word(one_long) (one_word & 0x0000FFFF)
#define hi_word(one_long) ((one_word & 0xFFFF0000) >> 16)

#define lo(one_word) (one_word & 0x00FF)
#define hi(one_word) ((one_word & 0xFF00) >> 8)


// OUTPUTS 
#define CS_DDR_MASK ((CS_ACCEL) | (CS_GYRO) | (CS_MAGNET))
#define CS_DDR  DDRD
#define CS_PORT PORTD

// INPUTS:
#define DRDY 30

#define A3 14
#define A4 18
#define A5 16
#define A6 26
#define A7 27
#define A8 17

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
