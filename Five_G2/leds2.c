#include <avr/io.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "leds2.h"


struct sSystemLED System_LED_Control;

/*
Want a way to :
	USE LED_1 & LED_2 when in  System_LED>0   mode  AND MOTOR_OFF!
	Do not disturb moving motors even if system mode is turned on!
	
	DDR will always be output for both and is set in the pwm_init() code.		
*/
void init_leds()
{
	// OUTPUTS:
	System_LED_Control.systemMode = SYSTEM_LED_MODE_DEVICE;
	System_LED_Control.pattern    = 0;

	// DDR =1 ==> output for Atmel AVRs

	LED_1_DDR |= (1<<LED_1);
	LED_2_DDR |= (1<<LED_2);
	LED_3_DDR |= (1<<LED_3);
	LED_4_DDR |= (1<<LED_4);

  	LED_1_OFF();
	LED_2_OFF();
	LED_3_OFF();
	LED_4_OFF();
}


void led_on(uint8_t mindex)
{
	//if (System_LED_Control.systemMode)	return;		// no operations in system mode!
	switch(mindex)
	{
		case 0:  LED_1_ON();  break;
		case 1:  LED_2_ON();  break;
		case 2:  LED_3_ON();  break;
		case 3:  LED_4_ON();  break;
		default: break;
	}
}

void led_off(uint8_t mindex)
{
  switch(mindex)
  {
   case 0:  LED_1_OFF();  break;
   case 1:  LED_2_OFF();  break;
   case 2:  LED_3_OFF();  break;
   case 3:  LED_4_OFF();  break;
   default: break;
  }
}

void set_led( uint8_t mindex, uint8_t value )
{
    //if (System_LED_Control.systemMode)	return;		// no operations in system mode!
	//protected_set_led( mindex, value );
}

/********************************************************************
Light the indicator LEDs to show the bit pattern specified.
INPUT:
	result	  : the uint8_t to show (note only 1/2 of this can be shown at a time)
	mHiNibble : TRUE => show the upper nibble
				FALSE => show the lower nibble
********************************************************************/
void protected_show_uint8_t( uint8_t result, uint8_t mHiNibble )
{
  if (mHiNibble) {
	  if ((result & 0x80)>0)  LED_4_ON()  else LED_4_OFF();
	  if ((result & 0x40)>0)  LED_3_ON()  else LED_3_OFF();
	  if ((result & 0x20)>0)  LED_2_ON()  else LED_2_OFF();
	  if ((result & 0x10)>0)  LED_1_ON()  else LED_1_OFF();
  } else {
	  if ((result & 0x08)>0)  LED_4_ON()  else LED_4_OFF();
	  if ((result & 0x04)>0)  LED_3_ON()  else LED_3_OFF();
	  if ((result & 0x02)>0)  LED_2_ON()  else LED_2_OFF();
	  if ((result & 0x01)>0)  LED_1_ON()  else LED_1_OFF();
  }
}

/* Not to be called by any user outside of this file! */
void protected_set_led( uint8_t mindex, uint8_t value )
{
	if (value)
		led_on (mindex);
	else
		led_off(mindex);
}

/* Useful for Verifying Pin connections (Initial board test)
	Return :  TRUE -> Complete scan
			  FALSE -> Still more leds to light
*/
uint8_t strobe_leds( uint8_t mOn )
{
	static uint8_t counter= STROBE_SPEED;	
	static uint8_t i=1;

	if ((System_LED_Control.systemMode == SYSTEM_LED_MODE_DEVICE) ||
		(System_LED_Control.systemMode == SYSTEM_LED_MODE_STROBE))
	{
		if (counter-->0)	return FALSE;
		counter = STROBE_SPEED;
		if (mOn) led_on( i ); else led_off( i );
		i++;
		if (i>=5)  { i=1; return TRUE; }
	}
	return FALSE;
}

/********************************************************************
Light the indicator LEDs to show the bit pattern specified.
INPUT:
	result	  : the uint8_t to show (note only 1/2 of this can be shown at a time)
	mHiNibble : TRUE => show the upper nibble
				FALSE => show the lower nibble
********************************************************************/
void show_uint8_t( uint8_t result, uint8_t mHiNibble )
{
	if (System_LED_Control.systemMode)	{
	  result = System_LED_Control.pattern;		// we override the value with the system requested pattern.
	}
    protected_show_uint8_t( result, mHiNibble );
}

uint8_t hi_low = 1;
void show_result_toggle( uint8_t mresult )
{
	if ((System_LED_Control.systemMode == SYSTEM_LED_MODE_DEVICE) ||
		(System_LED_Control.systemMode == SYSTEM_LED_MODE_MYINSTANCE))
	{	// no operations in system mode!
		if (hi_low)
		  protected_show_uint8_t( mresult, 1 );
		else 
		  protected_show_uint8_t( mresult, 0 );
		hi_low = ! hi_low;		// toggle
	}
}


#define BLINK_RATE 10
uint8_t    LED_state = 0;

void led_timeslice_100ms()
{
	static uint8_t counter = 1;
	switch (System_LED_Control.systemMode)
	{
	case SYSTEM_LED_MODE_DEVICE 	: 
		/*switch(led_Mode)
		{case STROBE :  break;
		default: break;		}*/
									  break;
	case SYSTEM_LED_MODE_PATTERN 	: show_uint8_t(System_LED_Control.pattern, LOW_NIBBLE);	
									  break;
	case SYSTEM_LED_MODE_STROBE 	: if (strobe_leds( System_LED_Control.pattern ) == TRUE)
											System_LED_Control.pattern = !System_LED_Control.pattern;
									  break;
	case SYSTEM_LED_MODE_MYINSTANCE : if (counter--==0) {
										counter = BLINK_RATE;
										//System_LED_Control.pattern= MyInstance; already done.
//										show_result_toggle( MyInstance );
									}
									break;
	default :   break;
	}
}


