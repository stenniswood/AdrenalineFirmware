
#ifndef _LEDS_H_
#define _LEDS_H_

void configure_leds		(			);
void led_on				(byte mindex);
void led_off			(byte mindex);
void set_led			(byte mindex, byte value);
void show_byte			(byte result, byte mHiNibble );
void show_result_toggle (byte mresult 				 );


#endif
