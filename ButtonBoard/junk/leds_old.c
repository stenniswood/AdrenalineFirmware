#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "leds.h"


#define SHOW_DELAY 400000

void configure_leds()
{ 
  LED_DDR |= LED_DDR_MASK;
}

void led_on(byte mindex)
{
  switch(mindex)
  {
   case 1:  SET_LED_1;  break;
   case 2:  SET_LED_2;  break;
   case 3:  SET_LED_3;  break;
   case 4:  SET_LED_4;  break;
   default: break;
  }
}

void led_off(byte mindex)
{
  switch(mindex)
  {
   case 1:  RESET_LED_1;  break;
   case 2:  RESET_LED_2;  break;
   case 3:  RESET_LED_3;  break;
   case 4:  RESET_LED_4;  break;
   default: break;
  }
}

void set_led(byte mindex, byte value)
{
	if (value)
		led_on(mindex);
	else
		led_off(mindex);
}

void show_byte( byte result, byte mHiNibble )
{
  if (mHiNibble) {
	  if ((result & 0x80)>0)  SET_LED_4  else RESET_LED_4 ;
	  if ((result & 0x40)>0)  SET_LED_3  else RESET_LED_3 ;
	  if ((result & 0x20)>0)  SET_LED_2  else RESET_LED_2 ;
	  if ((result & 0x10)>0)  SET_LED_1  else RESET_LED_1 ;
  } else {
	  if ((result & 0x08)>0)  SET_LED_4  else RESET_LED_4 ;
	  if ((result & 0x04)>0)  SET_LED_3  else RESET_LED_3 ;
	  if ((result & 0x02)>0)  SET_LED_2  else RESET_LED_2 ;
	  if ((result & 0x01)>0)  SET_LED_1  else RESET_LED_1 ;    
  }
}

byte hi_low = 1;
void show_result_toggle( byte mresult )
{
	if (hi_low)	
      show_byte( mresult, 1 );
	else 
      show_byte( mresult, 0 );
	
	hi_low = ! hi_low;		// toggle
}
