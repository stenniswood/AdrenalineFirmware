/*********************************************************************
 *    DESCRIPTION: Combined Backlight, Buzzer, LightSense module.	 *
 *					backlight can be set via CAN msg or to a Light	 *
 *					sensor based mode.		  						 *
 *											   						 *
 *	  CATEGORY:   BIOS Driver										 *
 *											   						 *
 *    PURPOSE: 														 *
 *			a) Uses PWM core module to provide a signal to backlight pin. *
 *			b) Reads LightSensor from ADC module.					 *
 *			c) Provides beeping pulses	(fixed on/off times)		 *
 *																	 *
 *	  PIN ASSIGNMENTS:												 *
 *				See "pin_definitions.h"								 *
 *																	 *
 *    AUTHOR: Stephen Tenniswood, Software Engineer 	 			 *
 *			  Sept 20, 2013						 				 	 *
 *********************************************************************/
//#include "iom16m1.h"
#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "../core/leds.h"
#include "../core/can.h"
#include "can_msg.h"
#include "buttons.h"
#include "fonts/allFonts.h"
#include "../core/pwm.h"
#include "../core/bk_system_defs.h"
#include "backlight.h"

word BeepOn_ms  = 0;
word BeepOff_ms = 0;
byte BeepState  = BEEP_OFF;
word NumBeeps   = 0;
word tics     = 0;

/*void backlight_full_on_off( byte mOn )
{	
	if (mOn)
		LCD_CONTROL_PORT |= (1<<LCD_BACKLIGHT);
	else
		LCD_CONTROL_PORT &= ~(1<<LCD_BACKLIGHT);
}*/

void buzz( byte mOn )
{
	if (mOn)
		BUZZER_PORT |= BUZZER_MASK;
	else
		BUZZER_PORT &= ~BUZZER_MASK;	
}

void backlight_init()
{
	// SET OUTPUT:
	BUZZER_DDR		 |= BUZZER_MASK;
	LIGHT_SENSOR_DDR &= ~LIGHT_SENSOR_MASK;

	// adc_init();
	pwm_init( 5000., FALSE, TRUE );
	set_dutyB(0.35);
}

void set_brightness( float mDuty )
{
	set_dutyB( mDuty );
}

void set_beep_times( word mOn_ms, word mOff_ms, byte mNumBeeps )
{
	BeepOn_ms  = mOn_ms;
	BeepOff_ms = mOff_ms;
	NumBeeps  = mNumBeeps;
	BeepState = BEEP_ON;
	tics      = 0;
	buzz( 1 );
}

// Call every 1ms
void backlight_timeslice()
{	
	tics++;
	
	switch(BeepState)
	{
	case BEEP_ON:	
			if (tics == BeepOn_ms) {
				buzz( 0 );
				tics = 0;
				NumBeeps--;
				BeepState = BEEP_SILENT;
				if (NumBeeps<=0)
					BeepState = BEEP_OFF;
			}
			break;	
	case BEEP_SILENT:
			if (tics == BeepOff_ms) {	
				buzz( 1 );
				tics  = 0;
				BeepState = BEEP_ON;
			}
			break;
	case BEEP_OFF:
			break;
	default:	
			break;
	}
}


