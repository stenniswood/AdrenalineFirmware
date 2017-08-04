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
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>

#include "inttypes.h"
#include "pin_definitions.h"
#include "motor.h"
#include "pot.h"
#include "pwm.h"
#include "adc.h"
#include "leds2.h"
//#include "calibrations.h"
struct sEndPoint
{
	long int angle;		// FPA type;  Degrees*100  
	uint16_t 	 value;		// Pot value
};
struct sEndPoint 	EndPoint1;
struct sEndPoint 	EndPoint2;
struct sEndPoint 	Range;			// Computed from EndPoint1 & 2


/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
short PotSpeed[2];
uint16_t PotAcceleration[2];

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
	Destination.continuous = 1;

	// SET AS INPUT:	
	CURRENT_SENSE_DDR  &= ~((1<<CURRENT_SENSE_LEFT) | (1<<CURRENT_SENSE_RIGHT));
	CURRENT_SENSE_PORT &= ~((1<<CURRENT_SENSE_LEFT) | (1<<CURRENT_SENSE_RIGHT));

	// SET AS INPUT:
	POT_DDR  &= ~(1<<POT);
	POT_PORT &= ~(1<<POT);

	compute_range();
}

/**********************************************
pot_timeslice()
	This is called from motor_timeslice() so the same frequency
	Note however, that the sample buffer is updated every interrupt.
	
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
uint16_t get_pot()
{
	return PotSample[1];
}
/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
uint16_t get_distance_to_destination()
{
	return (Destination.position - PotSample[1]);
}
/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
byte get_direction_to_destination()
{
	if (Destination.position > PotSample[1]) {
		Destination.direction = 1;
		return 1;
	}
	else if (Destination.position < PotSample[1]) {
		Destination.direction = -1;	
		return -1;
	}
	else {
		Destination.direction = 0;
		return 0;	// exactly at destination
	}
}

/**********************************************
Return	:	The motor position in degrees * 10
***********************************************/
void compute_pot_speed()
{
// Problem is Sample Rate versus Report Rate.
// Sample rate must be so high we only see 1 or 2 at the most difference.
	PotSpeed[0] = PotSpeed[1];
	PotSpeed[1] = (PotSample[1]-PotSample[0]);
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
	uint16_t half = (Range.value>>2) + EndPoint1.value;
	
	float delta    = (position - half);
	float percent  = (delta / (float)half);
	return percent;
}


/*********** "STOP" FUNCTIONS *******************************************/
/*******************************************************
convert_to_angle( uint16_t  mPotValue )
RETURN	:	Takes a Pot reading and converts to an angle
	based on the already calibrated stops.
******************************************************/
float convert_to_angle( uint16_t  mPotValue )
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
uint16_t  convert_to_value( long mAngleHundredthDeg )
{
	// Linear Interpolate:  (Value / ValueRange) = (angle / angleRange)
	float ratio 	= (mAngleHundredthDeg - get_min_stop_angle()) / Range.angle;
	float pot_value = (ratio * Range.value) + get_min_stop_value();
	return (uint16_t)pot_value;
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
uint16_t	  get_min_stop_value()
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
uint16_t	  get_max_stop_value()
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
	uint16_t Min = get_min_stop_value();
	if (EndPoint1.value == Min) return;		// okay

	// SWAP EP1 & EP2:
	long int tangle = EndPoint1.angle;
	uint16_t     tvalue = EndPoint1.value;
	
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
	uint16_t Min = get_min_stop_angle();
	if (EndPoint1.angle == Min) return;		// okay

	// SWAP EP1 & EP2:
	long int tangle = EndPoint1.angle;
	uint16_t     tvalue = EndPoint1.value;
	
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
byte  check_stops( uint16_t  mPotValue )
{
	static byte previous_state = 0;
	led_off(LIMIT_MIN);
	led_off(LIMIT_MAX);

	if (previous_state==1)		// IN STOP 1
	{
		uint16_t hysteresis = EndPoint1.value + HYSTERESIS_THRESHOLD;
		if (mPotValue > hysteresis) {
			previous_state = 0;
			return 0;
		}
		else {
			led_on(LIMIT_MIN);
			return previous_state;		
		}
	}
	if (previous_state==2)		// IN STOP 2
	{
		uint16_t hysteresis = EndPoint2.value - HYSTERESIS_THRESHOLD;
		if (mPotValue < hysteresis) {
			previous_state = 0;
			return 0;
		}
		else {
			led_on(LIMIT_MAX);		
			return previous_state;		
		}
	}
		
	if (mPotValue < EndPoint1.value)
	{
		led_on(LIMIT_MIN);
		previous_state = 1;
		return 1;
	}
	if (mPotValue > EndPoint2.value)
	{
		led_on(LIMIT_MAX);
		previous_state = 2;
		return 2;
	}
	return 0;
}

float current_2_threshold=50;

/**********************************************
// Motor Current above threshold?
Return	:	0 = Normal Operating Range
			1 = Over current Left side
			2 = Over current Right side
***********************************************/
byte	check_stall( uint16_t  mLeftCurrent, uint16_t mRightCurrent )
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
	if (Destination.continuous)	return 0;

	if (Destination.position == PotSample[1])
		return 1;

	if (previous_state==2)
	{
		uint16_t hysteresis = Destination.position - (0.1*Destination.position);
		if (PotSample[1] < hysteresis) {
			previous_state = 0;
			return 0;
		}
		else return previous_state;		
	}

	if (previous_state==3)
	{
		uint16_t hysteresis = Destination.position + (0.1*Destination.position);
		if (PotSample[1] > hysteresis) {
			previous_state = 0;
			return 0;
		}
		else return previous_state;
	}

	// Check for overage
	if (Destination.starting_position < Destination.position)
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




