
#ifndef _LEDS_H_
#define _LEDS_H_


#define SET_LED_1() 	{ LED_PORT |= (1<<LED_1); 	}
#define SET_LED_2() 	{ LED_PORT |= (1<<LED_2); 	}
#define SET_LED_3() 	{ LED_PORT |= (1<<LED_3); 	}
#define SET_LED_4() 	{ LED_PORT |= (1<<LED_4); 	}

#define RESET_LED_1() 	{ LED_PORT &= ~(1<<LED_1); 	}
#define RESET_LED_2() 	{ LED_PORT &= ~(1<<LED_2); 	}
#define RESET_LED_3() 	{ LED_PORT &= ~(1<<LED_3); 	}
#define RESET_LED_4() 	{ LED_PORT &= ~(1<<LED_4); 	}


void init_leds			(			);
void led_on				(byte mindex);
void led_off			(byte mindex);
void set_led			(byte mindex, byte value);
void show_byte			(byte result, byte mHiNibble );
void show_result_toggle (byte mresult 				 );


#endif
