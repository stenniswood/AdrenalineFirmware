/* pins_arduino.h for the adrenaline_device_atmega16m1 board has been
done such that the numbers correspond 1 for 1 with the chip.  
*/
#ifndef _PIN_DEFS_
#define _PIN_DEFS_
//#include <avr/io.h>

#ifdef __cplusplus
extern "C"{
#endif

#define _far 

#define byte unsigned char
#ifndef word
#define word unsigned short
#endif
#define BOOL byte
#define TRUE  1
#define FALSE 0

#define BOARD_MODEL Adrenaline_LCD

/********************* BOARD REVISION 1 ***************/
#define DELAY 100

// LCD BOARD LEDS:
#define LED_1_2_USEABLE		1
#define LED_1 			6
#define LED_2 			7
#define LED_3 			4
#define LED_4 			3
#define LED_DDR_MASK 	((1<<LED_1) | (1<<LED_2) | (1<<LED_3) | (1<<LED_4))
#define LED_1_DDR 		DDRD
#define LED_2_DDR 		DDRD
#define LED_3_DDR 		DDRD
#define LED_4_DDR 		DDRD

#define LED_1_PORT 		PORTD
#define LED_2_PORT 		PORTD
#define LED_3_PORT 		PORTD
#define LED_4_PORT 		PORTD

#define LIGHT_SENSOR_PIN 	5
#define LIGHT_SENSOR_DDR 	DDRD
#define LIGHT_SENSOR_PORT 	PORTD
#define LIGHT_SENSOR_MASK 	(1<<LIGHT_SENSOR_PIN)

#define CONTRAST_PIN 	2
#define CONTRAST_DDR 	DDRD
#define CONTRAST_PORT 	PORTD
#define CONTRAST_MASK 	(1<<CONTRAST_PIN)

#define BUZZER_PIN 			1
#define BUZZER_DDR 			DDRD
#define BUZZER_PORT 		PORTD
#define BUZZER_MASK			(1<<BUZZER_PIN)

#define LCD_DATA_BUS_PORT 	PORTB
#define LCD_DATA_BUS_INPORT PINB
#define LCD_DATA_BUS_DDR  	DDRB

#define LCD_CONTROL_PORT	PORTC
#define LCD_CONTROL_DDR		DDRC
#define LCD_RESET 			0
#define LCD_BACKLIGHT		1
#define LCD_RW				4
#define LCD_DI				5
#define LCD_CS_2			7
#define LCD_CS_1			6
#define LCD_CONTROL_MASK	((1<<LCD_RESET) | (1<<LCD_BACKLIGHT) | (1<<LCD_R_W) | (1<<LCD_D_I) | (1<<LCD_CS_1) | (1<<LCD_CS_2))

#define set_lcd_bit( mBit, mValue )  { if (mValue) LCD_CONTROL_PORT |= (1<<mBit); else LCD_CONTROL_PORT &= ~(1<<mBit); };

#define LCD_ENABLE 			0
#define LCD_ENABLE_PORT 	PORTD
#define LCD_ENABLE_DDR 		DDRD
#define LCD_ENABLE_MASK		(1<<LCD_ENABLE)

#define RANDOM_NUMBER_ADC_MUX 0x43		// most use LED3

// OUTPUTS 

// INPUTS:


#ifdef __cplusplus
} // extern "C"
#endif

#endif

