#ifndef _LEDS2_H_
#define _LEDS2_H_

#include "global.h"

///////////////////////////////////////
// LED INDICATORS : 
///////////////////////////////////////

#define STOP_LED 	LED_2
#define GO_LED 		LED_4
#define SERIAL_LED 	LED_3
#define WARNING_LED LED_1

#define LED_1		0
#define LED_1_DDR	DDRG				/* a 1 bit ==> output */
#define LED_1_PORT  PORTG				

#define LED_2		1					// RED
#define LED_2_DDR	DDRG
#define LED_2_PORT  PORTG

#define LED_3		2					// YELLOW
#define LED_3_DDR	DDRG
#define LED_3_PORT  PORTG

#define LED_4		3					// GREEN
#define LED_4_DDR	DDRG
#define LED_4_PORT  PORTG


/******************************************************* 
 SYSTEM LED COMMAND STRUCTURE 
*******************************************************/
#define SYSTEM_LED_MODE_DEVICE		 0x00
#define SYSTEM_LED_MODE_PATTERN		 0x01
#define SYSTEM_LED_MODE_STROBE		 0x02
#define SYSTEM_LED_MODE_MYINSTANCE	 0x03
#define SYSTEM_LED_MODE_CONT_PATTERN 0x04

struct sSystemLED {
	uint8_t pattern;			// 
	uint8_t systemMode;		// See above #defines
};
extern sSystemLED System_LED_Control;


// Pin_definitions.h must define : 
/*   LED_n_PORT, LED_n_DDR, and LED_n  		*/

#define LED_1_ON() 		{ LED_1_PORT |=  (1<<LED_1); }
#define LED_1_OFF() 	{ LED_1_PORT &= ~(1<<LED_1); }

#define LED_2_ON() 		{ LED_2_PORT |=  (1<<LED_2); 	}
#define LED_2_OFF() 	{ LED_2_PORT &= ~(1<<LED_2); 	}

#define LED_3_ON() 		{ LED_3_PORT |=  (1<<LED_3); 	}
#define LED_3_OFF() 	{ LED_3_PORT &= ~(1<<LED_3); 	}

#define LED_4_ON() 		{ LED_4_PORT |=  (1<<LED_4); 	}
#define LED_4_OFF() 	{ LED_4_PORT &= ~(1<<LED_4); 	}


#define HIGH_NIBBLE 1
#define LOW_NIBBLE  0

uint8_t strobe_leds		( uint8_t mOn 	);
void init_leds			(			);
void led_on				(uint8_t mindex);
void led_off			(uint8_t mindex);
void set_led			(uint8_t mindex, uint8_t value);
void show_uint8_t			(uint8_t result, uint8_t mHiNibble );
void show_result_toggle (uint8_t mresult 				 );
void led_timeslice_100ms(							 );

//void can_parse_led_request( sCAN* mMsg );


#endif
