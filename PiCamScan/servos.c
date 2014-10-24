/*********************************************************************
Product of Beyond Kinetics, Inc

This code handles Lowside drivers hardware.

	There are 7 outputs going into the ULN2003A darlington array.
	Each transistor rated at 500mA.  (So the high side should come directly from the power source, 
		and not the regulated voltage (5V).  This allows a higher voltage (more flexible circuits)
		and doesn't burden the 2A regulator.

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <avr/sfr_defs.h>
#include <math.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "leds.h"
#include "buttons.h"
#include "can_id_list.h"
#include "can_instance.h"
#include "can_eid.h"
#include "can_board_msg.h"
#include "pwm.h"
#include "servos.h"


int MinCount = 0;
int MaxCount = 1;

float ServoRangeDegs;
float ServoCountRange;
word  CenterCount;

void servos_init()
{
	pwm_init( BASE_FREQ, TRUE, TRUE );
	// How many counts do we get at this base frequency?
	// 3,200  see "get_top_value()" in pwm.c.  It's 16,000,000/8/50 = 40,000
	// Good enough resolution.

	//byte ps               = select_prescaler( BASE_FREQ );
	float counts_per_period = get_top_value( BASE_FREQ );
	float period_ms         = (1.0 / BASE_FREQ) * 1000.0;		// in ms 
	float counts_per_ms     = counts_per_period / period_ms;

	MinCount = round(counts_per_ms * (float)RC_MIN_PW_TIME_MS);
	MaxCount = round(counts_per_ms * (float)RC_MAX_PW_TIME_MS);

	ServoRangeDegs  = (RC_MAX_ANGLE - RC_MIN_ANGLE);
	ServoCountRange = (MaxCount - MinCount);
	CenterCount     = ((float)counts_per_ms * RC_NEUTRAL_PW_TIME_MS );	
}

void move_to_min(byte mServoNum)
{
	if (mServoNum==1)
		OCR1A_Write( MinCount );
	else
		OCR1B_Write( MinCount );	
}
void move_to_max(byte mServoNum)
{
	if (mServoNum==1)
		OCR1A_Write( MaxCount );
	else
		OCR1B_Write( MaxCount );
}

// Linearly interpolate:
// mDegrees	/ mCount = ServoRangeDegs / ServoCountRange
// mCount / mDegrees = ServoCountRange / ServoRangeDegs
// mCount   = ServoCountRange / ServoRangeDegs * mDegrees

// 0.5ms 1.0 ms  2.5ms
void set_angle(byte mServoNum, float mDegrees)
{
	if (mDegrees > RC_MAX_ANGLE) return;
	if (mDegrees < RC_MIN_ANGLE) return;
	
	word Count = round(((float)(ServoCountRange/ServoRangeDegs)*mDegrees)) + (ServoCountRange/2.0) + MinCount;
/*
(MaxCount - MinCount) 
(RC_MAX_ANGLE - RC_MIN_ANGLE)
counts_per_degree * mDegrees + 
*/
	if (mServoNum==1)
		OCR1A_Write( Count );
	else
		OCR1B_Write( Count );
}

void neutral( byte mServoNum )
{
	if (mServoNum==1)
		OCR1A_Write( CenterCount );
	else 
		OCR1B_Write( CenterCount );
}

float counts_per_period;
word c;
word limit;
void forward()
{
    counts_per_period = get_top_value( BASE_FREQ );
	limit  = round( counts_per_period / 8.0 );
	c      = round( counts_per_period / 22.0 );
	while (c < limit)
	{
		OCR1A_Write( c );
		OCR1B_Write( c );
		c++;
		delay(400);
	}
}
void backward()
{
    counts_per_period = get_top_value( BASE_FREQ );
	limit = round( counts_per_period / 22.0 );
	while (c > limit)
	{
		OCR1A_Write( c );
		OCR1B_Write( c );
		c--;
		delay(400);
	}
}

	/* WAIT a minute!  That defines a 0..100% duty.  
		FarLeft  = 0.5 * 160 = 80
		Neutral would be x/1ms = 3200/20ms = 160 counts for neutral position.
		FarRight = 2.5ms*3200/20ms = 400 
	
		The best we can do is 3200 counts.  Think if we use 65535 counts to define the RC period (20ms).
		then the neutral pulse 1ms will be 1/20th of 65535 = 3276.
		If we slow down the prescaler, then there will be fewer counts than 65535 per period.
			thus fewer also per pulse time.
		Only way to zoom in on the pulse would be to alternate a zero duty every other TOP overflow.
		Then we'd have 65535 counts per 10ms; and 6,553 per ms.
		or 4x zoom:  20ms / 4 = 5ms.  So 3 frames of zero duty cycle and 65535 counts per 5ms
		65535 / 5 = 13,107 counts per 1 ms (neutral).
		That's good resolution!
		
		Let's start by not doing this!!  :)
		
		Simple test low resolution 80 counts from Left to Neutral.  and 240 counts neutral to right.
				
		With Prescaler=1;  Then 
		With Prescaler=8;  Then 40k counts per 20ms;  which leaves 2,000 counts per ms.
	*/
