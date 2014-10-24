/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Low Level Routines RTOS					 	 	 *
 *											  						 *
 *	  CATEGORY:   BIOS Driver										 *
 *											   						 *
 *    PURPOSE: Use Timer 0 for a 1ms tic.  Interrupt handler to 	 *
 *           set update counters, then dispatch any tasks.			 *
 *																	 *
 *	  HARDWARE RESOURCES:									   		 *
 *			 Atmega16m1 Timer 0 is set up as an 8bit PWM	 		 *
 *																	 *
 *	  PIN ASSIGNMENTS:												 *
 *           No external pins 						 				 * 
 *											   						 *
 *    AUTHOR: Stephen Tenniswood, Robotics Engineer 	 			 *
 *		  	  December 2007						 				 	 *
 *		  	  Revised for atmel Sept 2013						 	 *
 *********************************************************************/
#include <stdlib.h>
//#include <math.h>
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "motor.h"
#include "can_instance.h"
#include "pot.h"
#include "pwm.h"
#include "eeprom.h"
#include "adc.h"
#include "calibrations.h"


/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
word PotSpeed[2];
word PotAcceleration[2];

struct sEndPoint 	minStop;		// Sorted versions of the EndPoint1&2
struct sEndPoint 	maxStop;		// Sorted versions of the EndPoint1&2
struct sDestination Destination;

/*********** FUNCTIONS *******************************************/



/* Set up the ADC and DDR for current sense and pot	*/
/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
void pot_init()
{
	Destination.Continuous = 1;

	// SET AS INPUT:	
	CURRENT_SENSE_DDR  &= ~((1<<CURRENT_SENSE_LEFT) | (1<<CURRENT_SENSE_RIGHT));
	CURRENT_SENSE_PORT &= ~((1<<CURRENT_SENSE_LEFT) | (1<<CURRENT_SENSE_RIGHT));

	// SET AS INPUT:
	POT_DDR  &= ~(1<<POT);
	POT_PORT &= ~(1<<POT);
		
	adc_init();	
	start_sampling();		// Need to call 1 Time to get it started (ISR will keep going)
	compute_range();		
}
/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
void pot_timeslice()
{
	compute_pot_speed();
	compute_pot_acceleration();
}

/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
word get_pot()
{
	return PotSample[1];
}
/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
word get_distance_to_destination()
{
	return PotSample[1];
}

/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
void compute_pot_speed()
{
	PotSpeed[0] = PotSpeed[1];
	PotSpeed[1] = (PotSample[1] - PotSample[0]);
}
/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
void compute_pot_acceleration()
{
	PotAcceleration[0] = PotAcceleration[1];
	PotAcceleration[1] = (PotSpeed[1] - PotSpeed[0]);
}
/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
long int  get_pot_angle()
{
	return convert_to_angle(PotSample[1]);
}

/*******************************************************
 pot_percent()
RETURN	:	float => Percentage between [0.0 .. 1.0]
This is calculated for a 50% duty being no motor movement.
Since BigMotor PWMs the High/Low signal, this is correct.
Forward is duty > 0.50
Backward is duty [0.0.. 0.5]
******************************************************/
#define MID_WAY ((float)0x1FF)
float pot_percent()
{
	float position = PotSample[1];
	word half = (Range.value>>2) + EndPoint1.value;
	
	float delta    = (position - half);
	float percent  = (delta / (float)half);
	return percent;
}


/*********** "STOP" FUNCTIONS *******************************************/
/*******************************************************
convert_to_angle( word  mPotValue )
RETURN	:	Takes a Pot reading and converts to an angle
	based on the already calibrated stops.
******************************************************/
float convert_to_angle( word  mPotValue )
{
	// Linear Interpolate:  (Value-MinValue) / ValueRange = (angle-MinAngle) / angleRange
	float ratio = (mPotValue-get_min_stop_value()) / Range.value;
	float angle = ( ratio * Range.angle );			// range has the *100 in it
	return angle + get_min_stop_angle();
}
long int convert_to_fixedpoint( float  mRealNumber )
{
	long int fp = (mRealNumber * 100);
	return fp;
}
/*******************************************************
convert_to_value( float mAngle )
RETURN	:	Takes an Angle and converts to an expected
	POT reading.
******************************************************/
word  convert_to_value( float mAngle )
{
	// Linear Interpolate:  (Value / ValueRange) = (angle / angleRange)
	float ratio 	= ((mAngle*100)-get_min_stop_angle()) / Range.angle;
	float angle 	= (ratio * Range.value) + get_min_stop_value();
	return angle;
}

/*******************************************************
 compute_default_stops()
RETURN	:	float => Percentage between [0.0 .. 1.0]

******************************************************/
void  compute_default_stops()
{
	EndPoint1.value = 25; 			//(0.1 * MIN_POT_VALUE);		// 10 % limits
	EndPoint2.value = 0x3ff-25; 	//(0.9 * MAX_POT_VALUE);		// 10 % limits	
	
	EndPoint1.angle =   0*100;		// 0 degrees
	EndPoint2.angle = 300*100;		// Most Pots turn 300 degrees
}

/*******************************************************
compute_range()
RETURN	:	Difference between endpoints. Stored in file
			variable.
******************************************************/
void  compute_range()
{
	Range.angle = (EndPoint2.angle - EndPoint1.angle);
	Range.value = (EndPoint2.value - EndPoint1.value);
}

/*******************************************************
get_min_stop_value()
RETURN	:	Lesser of the 2 stop endpoints.  Value only.
******************************************************/
word	  get_min_stop_value()
{
	return min( EndPoint2.value, EndPoint1.value);	
}
/*******************************************************
get_min_stop_value()
RETURN	:	Lesser of the 2 stop endpoints.  Value only.
******************************************************/
long int  get_min_stop_angle()
{
	return min( EndPoint2.angle , EndPoint1.angle);
}
/*******************************************************
get_max_stop_value()
RETURN	:	Greater of the 2 stop endpoints.  Value only.
******************************************************/
word	  get_max_stop_value()
{
	return max( EndPoint2.value, EndPoint1.value );
}
/*******************************************************
get_max_stop_angle()
RETURN	:	Greater of the 2 stop endpoints.  Angle only.
******************************************************/
long int  get_max_stop_angle()
{
	return max( EndPoint2.angle , EndPoint1.angle);
}
/*******************************************************
get_max_stop_angle()
RETURN	:	Greater of the 2 stop endpoints.  Angle only.
******************************************************/
void  order_stops_by_value( )
{
	word Min = get_min_stop_value();
	if (EndPoint1.value == Min) return;		// okay

	// SWAP EP1 & EP2:
	long int tangle = EndPoint1.angle;
	word     tvalue = EndPoint1.value;
	
	EndPoint1.angle = EndPoint2.angle;
	EndPoint1.value = EndPoint2.value;		
	EndPoint2.angle = tangle;
	EndPoint2.value = tvalue;	
}
/*******************************************************
get_max_stop_angle()
RETURN	:	Greater of the 2 stop endpoints.  Angle only.
******************************************************/
void  order_stops_by_angle( )
{
	word Min = get_min_stop_angle();
	if (EndPoint1.angle == Min) return;		// okay

	// SWAP EP1 & EP2:
	long int tangle = EndPoint1.angle;
	word     tvalue = EndPoint1.value;
	
	EndPoint1.angle = EndPoint2.angle;
	EndPoint1.value = EndPoint2.value;		
	EndPoint2.angle = tangle;
	EndPoint2.value = tvalue;
}

/**********************************************
// Gone past either stop 1 or 2?
Return	:	0 = Normal Operating Range
			1 = Under the Lower stop
			2 = Over the top stop
***********************************************/
#define HYSTERESIS_THRESHOLD 7
byte  check_stops( word  mPotValue )
{
	static byte previous_state = 0;
	RESET_LED_LIMIT_MIN();
	RESET_LED_LIMIT_MAX();

	if (previous_state==1)		// IN STOP 1
	{
		word hysteresis = EndPoint1.value + HYSTERESIS_THRESHOLD;
		if (mPotValue > hysteresis) {
			previous_state = 0;			
			return 0;
		}
		else {
			SET_LED_LIMIT_MIN();
			return previous_state;		
		}
	}
	if (previous_state==2)		// IN STOP 2
	{
		word hysteresis = EndPoint2.value - HYSTERESIS_THRESHOLD;
		if (mPotValue < hysteresis) {
			previous_state = 0;
			return 0;
		}
		else {
			SET_LED_LIMIT_MAX();
			return previous_state;		
		}
	}
		
	if (mPotValue < EndPoint1.value)
	{
		SET_LED_LIMIT_MIN();
		previous_state = 1;
		return 1;
	}
	if (mPotValue > EndPoint2.value)
	{
		SET_LED_LIMIT_MAX();
		previous_state = 2;
		return 2;
	}
	return 0;
}

/**********************************************
// Motor Current above threshold?
Return	:	0 = Normal Operating Range
			1 = Over current Left side
			2 = Over current Right side
***********************************************/
byte	check_stall( word  mLeftCurrent, word mRightCurrent )
{
	if (mLeftCurrent > current_2_threshold)
		return 1;
	if (mRightCurrent > current_2_threshold)
		return 2;
	return 0;
}

/**********************************************
 How to best determine to avoid over shooting
This is not a simplistic question.  Algorithm
to reduce torque which depends on angle w.r.t. 
gravity etc.  This will be expanded upon, however
for simplisticity, we will return numerical compare.

Destination values shall be in ADC counts only.
Implements hysteresis

Return	:	0 = Normal Operating Range
			1 = Exactly reached
			2 = Close over
			3 = Close under
***********************************************/
byte	  destination_reached(  )
{
	static byte previous_state=0;	
	if (Destination.Continuous)	return 0;
	
	if (Destination.position == PotSample[1])
		return 1;

	if (previous_state==2)
	{
		word hysteresis = Destination.position - (0.1*Destination.position);
		if (PotSample[1] < hysteresis) {
			previous_state = 0;
			return 0;
		}
		else return previous_state;		
	}

	if (previous_state==3)
	{
		word hysteresis = Destination.position + (0.1*Destination.position);
		if (PotSample[1] > hysteresis) {
			previous_state = 0;
			return 0;
		}
		else return previous_state;
	}

	// Check for overage
	if (Destination.starting_point < Destination.position)
	{
		// Approaching upward:
		if (PotSample[1] > Destination.position) {
			previous_state = 2;
			return 2;		// reached upwardly
		} 
	}
	else 
	{	// Approaching downward:
		if ( PotSample[1] < Destination.position ) {
			previous_state = 3;
			return 3;		// reached downwardly
		}
	}
	return 0;	// Keep going!
}




