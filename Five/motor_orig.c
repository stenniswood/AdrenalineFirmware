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
 *			 Atmega16m1 Timer 1 OCR1A/B are setup as 16bit PWM	 	 *
 *																	 *
 *																	 *
 *	NOTE:  There are different configurations for the PWM/HBridges	 *
 *		METHOD B:													 *
 *			The HBridge really only requires 1 PWM signal.			 *
 *			We hold one side HIGH while the PWM occurs on the Low of *
 *			the other side.		To stop the motor in this method,	 *
 *				need to stop pwm and put at full up or down.  (brake/coast)	 *
 *		Not only is this the right way to do it, it allows possibility
 *		of swithing PWM between High/Low and Enable for Rev 2 of the *
 *		board design.												 *
 *																	 *  
 *																	 *
 *	  PIN ASSIGNMENTS:												 *
 *           No external pins 						 				 * 
 *											   						 *
 *    AUTHOR: Stephen Tenniswood, Robotics Engineer 	 			 *
 *		  	  December 2007						 				 	 *
 *		  	  Revised for atmel Sept 2013						 	 *
 *********************************************************************/
/*********************************************************************
Product of Beyond Kinetics, Inc
This file puts together various other files:  
	pot.c calibrations.c encoder.c can_msg.c 
-------------------------------------------------------------------
DATE 	:  10/20/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include <avr/io.h>

#include "inttypes.h"
//#include "interrupt.h"
#include "pin_definitions.h"
#include "motor.h"
#include "leds2.h"
#include "pwm.h"
//#include "calibrations.h"
#include "encoder.h"
#include "pot.h"
//#include "can.h"
//#include "can_board_msg.h"
//#include "can_msg.h"
#include "encoder.h"
//#include "configuration.h"
#include "adc.h"


/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
float MotorDutyRequest = 0.00;	// set in can_msg.c;  [-1.0..+1.0]

byte MotorState = MOTOR_OFF;
short ReportCounter = 1;		// when zero sends update of the motor angle & currents

/**************** FUNCTIONS *************************************/

/************************************************************
SetBaseFrequency()	based on config_byte_1
*************************************************************/
void SetBaseFrequency( )
{
	//switch (getBaseFreq())
	switch (2)
	{
	// 244 is lowest with /1 prescaler!  The get_top_value() function
	// will encounter an overflow (16,000,000 / 100)=160,000 which is bigger than 'word'
		case 0:	pwm_init(  245., TRUE, TRUE );	break;
		case 1:	pwm_init(  300., TRUE, TRUE );	break;
		case 2:	pwm_init( 1000., TRUE, TRUE );	break;
		case 3:	pwm_init( 5000., TRUE, TRUE );	break;
		default: pwm_init(5000., TRUE, TRUE );  break;
	}
}

/************************************************************
motor_init()		
*************************************************************/
/* The Motor is off when:
		Both PWMs are LOW;		
		Both PWMs are HIGH;		(ie Polarities are the same)
		INHibit signal is LOW;
	Base Frequency of the BTS7960 chips can go upto 25khz	*/
void motor_init()
{
	INHIBIT_DDR  |=  INHIBIT_DDR_MASK;			// HBridge Enable lines (outputs)
	CURRENT_SENSE_DDR &=  ~((1<<CURRENT_SENSE_LEFT) | (1<<CURRENT_SENSE_RIGHT));

	SetBaseFrequency( );	// config_byte_1
	motor_enable(FALSE);

//	if (isConfigured(MODE_TILT_RESPONDER))
	{
		motor_enable( TRUE );
	}
	// Default stops computed on first run of the board.
	// See calibrations.c, "app_default_configuration()"
}

/************************************************************
motor_timeslice()		Call this every 10ms

Read Position			Also update Speed & Acceleration
update duty (if pid)	
compare to limit		
compare to destination	
send postion report		
turn off if shutdown	
*************************************************************/
void motor_timeslice_10ms()
{
	// Move to Angle updates will happen asynchronously
	word position;

	// Check for STOP conditions:
	// Note if the lights stop working, or this timeslice is not going at all,
	// 	it could be hung up on can_instance.  ie if there are not any other boards in the network.
	if (MotorState == MOTOR_REQUEST_PENDING)
	{
		//if (isConfigured(MODE_2_MOTORS)==FALSE)
		if (1)
		{
			motor_set_duty( Destination.speed );
		} 
		else 
		{	// 2 unidirectional motors (independant PWM dutys)
			INHIBIT_PORT &= ~(1<<INHIBIT_LEFT );		// want both low - pulldowns.
			INHIBIT_PORT &= ~(1<<INHIBIT_RIGHT);		// 0=> Low side active
			set_dutyA( Destination.speed  );		
			set_dutyB( Destination.speed2 );		// left side needs power too.  
		}
	}

	// READ LATEST POSITION:
	//if (isConfigured(MODE_USE_ENCODER) ) 
	if (1)
	{
		encoder_timeslice();
		position = EncoderCount; 	
	} else {
		position = get_pot(); 	
		// update speed & accel in new Pot_timeslice()
	}

	if (1)
	//if (isConfigured(MODE_FREE_SPIN)==FALSE)
		stop_action( position );		// check stops and stop motor if necessary

	if (destination_reached()) {		// destination works in free spin mode.
		motor_stop();					// just no stops!
	} 

	// STALL CURRENT?
	//if (check_stall(LeftCurrentSample[1], RightCurrentSample[1]) )
	// 	motor_enable(FALSE);

	// PID UPDATE:
/*	if (isConfigured(MODE_PID_CONTROL))
	{  
		float pid = get_distance_to_destination();
		pid /= Range.value;				// total distance between endpoints.
		motor_set_duty( pid );
	} */

	// Time To Report:
//	if (getReportRate() != MODE_SEND_UPDATES_NONE)
	if (1)
	{
		//if (ReportCounter==-1)
		//{ }
		//else 
		if ((ReportCounter--)==0)
		{
//			ReportCounter = getReportRate();		// Refill
//			if (isConfigured2(MODE_SEND_POSITION_RAW))
			{
				//can_prep_motor_values_raw( &msg1 );
				//can_send_msg_no_wait( 0, &msg1 );
			}
//			if (isConfigured2(MODE_SEND_POSITION_CALC))
			{
				//can_prep_motor_angle( &msg1    );
				//can_send_msg_no_wait( 2, &msg1 );	// MOB2 is also unused
			}
//			if (isConfigured2(MODE_SEND_STATUS))
			{
				//can_prep_motor_status( &msg1    );
				//can_send_msg_no_wait ( 4, &msg1 );	// MOB4 is also unused
			}
			//System_Send_Status 	= 1;	// will be sent on next can_board_100ms_TimeSlice()

			// NOTE:  The "previous samples" held in subsript [0] are only updated every
			// Report time.  This was decided so that it's easily understandable to another 
			// programmer (ie. the numbers visible on the CAN monitor add up (subtract for speed).
			// Also because the POT sample rate is far too fast to have a significant resolution
			// on the speed (only 1 or 2 difference at max!).  This report rate gives good resolution
			// and reasonable response time.
//			if (isConfigured(MODE_USE_ENCODER) ) 
			if (1)
			{
					encoder_timeslice();
			} else {
					pot_timeslice();
			}
			PotSample[0] 		  = PotSample[1];
		} 
	}

//	if (ShutDownState == SHUTDOWN_PENDING)
	{
//		if (isConfigured(MODE_USE_ENCODER))
//			save_cal();
		motor_enable(FALSE); 
		//cli();
		// micro to sleep mode:
	}
}

void stop_action(word mPosition)
{
	byte stop = check_stops( mPosition );
	if (stop)
		motor_stop();
	else 
		motor_enable(TRUE);
}

float get_motor_angle()
{
    float_array Angle;
    //if (isConfigured(MODE_USE_ENCODER))
    if (1)
    {
    	Angle.value = get_encoder_angle();
    } else {
    	Angle.value = get_pot_angle();
    }
    return Angle.value;
}


void motor_enable( BOOL mEnable )
{
	if (mEnable)
		MotorState = MOTOR_ENABLED;
	else 
		MotorState = MOTOR_DISABLED;
}

void motor_stop()
{
	MotorState = MOTOR_OFF;
	set_dutyA( 0.0 );
	set_dutyB( 0.0 );
}

void motor_brake( BOOL mEnable )
{ 
	set_dutyA(0.0);
	set_dutyB(0.0);
	
	INHIBIT_PORT &= ~(1<<INHIBIT_LEFT);		// active low!
	INHIBIT_PORT &= ~(1<<INHIBIT_RIGHT);

	MotorState = MOTOR_BRAKING;	
}
void motor_coast( BOOL mEnable )
{
	set_dutyA(0.0);
	set_dutyB(0.0);

	INHIBIT_PORT |= (1<<INHIBIT_LEFT );
	INHIBIT_PORT |= (1<<INHIBIT_RIGHT);
	MotorState = MOTOR_BRAKING;	
}
/* Driving incorrectly!!
	Right now, we pick the motor direction and set those pins on the BTS7960.
	Then we PWM the INHibit pin.  That pin however disables both High & Low side drivers!
	So the current going thru the motor has nowhere to go and we get the huge sparks.
	What we want is to PWM the IN pin on 1 side.  This will allow the current to recycle
	thru VCC or GND and the motor. (conducting loop) and all that power has a place to go.
	
	This however requires the traces to be cut and switched.  Rev C error.  
	
	FORWARD:
		INH_LEFT=1 and INH_RIGHT=1;
		HILOW_LEFT=0;  PWM HILOW_RIGHT - when 0 recirculates, when 1 drives.

	REVERSE:
		INH_LEFT=1 and INH_RIGHT=1;
		HILOW_LEFT=PWM- when 0 recirculates, when 1 drives.;  HILOW_RIGHT=0 	
*/
/*void set_motor_direction( BOOL mForward )
{
	if (mForward)
	{
		//HILOW_PORT |=  (1<<HILOW_LEFT );
		HILOW_PORT &= ~(1<<HILOW_LEFT);

	}
	else {
		HILOW_PORT &= ~(1<<HILOW_RIGHT);
		//HILOW_PORT |=  (1<<HILOW_RIGHT);
	} 
} */

/*********************************************
motor_set_duty( float mFraction )
There are multiple ways of driving the HBridge
Method A) Keep toggling High/Low on both (but opposite polarity) sides.
		  0.5 = stopped;  0.6=> slow forward; 0.9=> fast forward;
		  0.5 = stopped;  0.4=> slow backward; 0.1 => fast backward;
Method B) 
	We PWM the LOW side and hold the Highside positive.
	We change the PWM_Left & PWM_Right pins.
	
NOTE :	No change to the Enable lines in this function!
INPUT:	mPercent - [0.0 .. +1.00]
*********************************************/
void motor_set_duty( float mFraction )
{	
	if (mFraction>0)
	{
		set_dutyA( mFraction );		// left side needs power
		set_dutyB( 0.0 );
	}
	else 
	{
		set_dutyA( 0.0 );
		set_dutyB( -mFraction );		// right side needs power 
	}

	// INHIBIT is active LOW.  So it really acts like an ENABLE signal	
	INHIBIT_PORT |=  (1<<INHIBIT_RIGHT);
	INHIBIT_PORT |=  (1<<INHIBIT_LEFT);
}

/* Sets the Motor Direction and Strength

OC1A & OC1B drive the EN => High/Low side active pin.  50% duty will drive motor forward/backward with equal time.
We set up the PWM drive to give opposite polarities.  When left side is high, right side will be low.
The motor is always getting current this way.  however the back and forth ratios are what determine the speed.
Just a little different than the normal PWM of the enable pin with a set duty.

To advance, increase the duty for both of these simultaneously (remember opposites)

INPUT:	mPercent - [-1.00 .. +1.00]

void motor_set_duty_signed( float mSignedFraction )
{
	float duty_0_100 = (mSignedFraction + 1.0)/2.0;
	motor_set_duty( duty_0_100 );
}*/
/*void enable_left_side(BOOL mEnable) 
{
// Note: signal is actually an "Inhibit"
//	if (mEnable)
// set duty to 0 or 100%
		PORTB |=  (1<<HILOW_LEFT);
	else 
		PORTB &= ~(1<<HILOW_LEFT); 
}
void enable_right_side(BOOL mEnable)
{
// Note: signal is actually an "Inhibit"
//	if (mEnable)
		PORTB |=  (1<<ENABLE_RIGHT); 	// inhibit active low.  enable active high.
	else
		PORTB &= ~(1<<ENABLE_RIGHT); 
}*/
