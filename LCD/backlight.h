#ifndef _BACKLIGHT_H_
#define _BACKLIGHT_H_

// State Machine Values:
#define BEEP_OFF 	0
#define BEEP_ON 	1
#define BEEP_SILENT 2


void backlight_init	( );

void set_beep_times		  ( word mOn_ms, word mOff_ms, byte mNumBeeps );
void set_brightness		  ( float mDuty		);
void backlight_full_on_off( byte mOn		);
void buzz				  ( byte mOn 		);

void backlight_timeslice();		// not implemented yet (for beeps only)


#endif
