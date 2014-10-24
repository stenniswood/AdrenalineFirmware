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
/*********************************************************************
This code handles can messages specific to the BigMotor board

The complete 29 bit message consists of:	
21	MESSAGE_ID

8	INSTANCE_ID because there may be more than 1 button board put into the network
--------------
8	DEVICE_ID	identifies this board "Adrenaline Button"

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
Product of Beyond Kinetics, Inc
********************************************************************/
#include <avr/sfr_defs.h>
#include <math.h>
#include "inttypes.h"
#include "interrupt.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "buttons.h"
#include "pot.h"
#include "can.h"
#include "can_id_list.h"
#include "can_msg.h"
#include "can_eid.h"
#include "can_instance.h"
#include "adc.h"
#include "calibrations.h"
#include "encoder.h"
#include "motor.h"
#include "configuration.h"
#include "pot.h"


/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
uint8_t  size = sizeof(float);

/************************ UTILITY FUNCTIONS *******************************/
float extract_float_msg( byte* mdata )
{
	float_array 	floater;
	for (int i=0; i<4; i++)
		floater.array[i] = mdata[i];
	return floater.value;
}
word extract_word( byte* mData )
{
	long int retval = 0;
	retval += mData[0];
	retval += (mData[1]<<8 );
	return retval;
}
long int extract_long_int( byte* mData)
{
	long int retval = 0;
	retval += mData[0];
	retval += (mData[1]<<8 );
	retval += ((long)mData[2])<<16;
	retval += ((long)mData[3]<<24);
	return retval;
}

/******************************************************************************/
/************************ INCOMING MESSAGE PROCESSING *************************/
/******************************************************************************/
/* Callback function (from the CAN ISR()).  
	Parse the Msg based on the msg id. 
#define ID_MARK_MOTOR_STOP				0x0040		 1 or 2 in data[0]
		Set Stop 1	(Mark the current motor position as Stop 1 - angle given)
#define ID_MOVE_TO_ANGLE				0x0042		 Instance goes with intended Receiver
#define ID_MOVE_SPEED					0x0043
#define ID_SET_MAX_MOTOR_ACCEL			0x0044		*/
void can_file_message( sCAN* mMsg )
{
	if ( id_match( mMsg->id, create_CAN_eid(ID_MARK_MOTOR_STOP, MyInstance)) )
	{
		can_proc_set_stop_msg( mMsg );
		// Save EEPROM
		save_cal(  );	// defined in pot.c
	}
	else if ( id_match(mMsg->id, create_CAN_eid(ID_MOVE_TO_ANGLE, MyInstance)) )
	{
		// includes a speed
		can_proc_move_to_angle_msg( mMsg );
		// Initiate Motor
	}	
/*	else if ( id_match(mMsg->id, create_CAN_eid(ID_MOVE_SPEED, MyInstance)) )	
	{
		can_proc_move_speed_msg( mMsg );
	} */
	else if ( id_match(mMsg->id, create_CAN_eid(ID_SET_MAX_MOTOR_ACCEL, MyInstance)) )	
	{	
		can_proc_max_acceleration_msg( mMsg );
	}
	else if ( (isConfigured(MODE_TILT_RESPONDER)) && 
			  (id_match(mMsg->id, create_CAN_eid(ID_ACCEL_XYZ, 0)) ) )
	{
		can_proc_tilt_msg( mMsg );
		
	}
}

/* Extract angle from the Message */
#define KConst 0.25
void can_proc_tilt_msg( sCAN* mMsg )
{
	byte yh  = mMsg->data[3];
	byte yl  = mMsg->data[2];
	short y1  = ((yh<<8) | yl);
	float y2 = ((float)y1 / (16535.0));

//  short y2 = (short)y1; 					//hi(y1) + (lo(y1)<<8);
//	float tip = ((float)y2) / 22767.0;		// 0x3FFF  about 1 g	
	//word z = extract_word( &(mMsg->data[4]) );	

	// multiply by proportionality constant

	// We don't actually set the motor duty here, because we don't know if it's in a stop region
	// or under influence of some other condition (ie pid mode, etc).  So we queue up a 
	// request and let the timeslice deal with it.
	MotorState 		 = MOTOR_REQUEST_PENDING;
	MotorDutyRequest = y2;
}

/* Extract angle from the Message */
void can_proc_set_stop_msg( sCAN* mMsg )
{
	if (mMsg->data[0] == 1)
	{
		EndPoint1.angle = extract_long_int( &(mMsg->data[1]) );
		EndPoint1.value = extract_word	  ( &(mMsg->data[4]) );
	} else {
		EndPoint2.angle = extract_long_int( &(mMsg->data[1]) );
		EndPoint2.value = extract_word	  ( &(mMsg->data[4]) );
	}
	order_stops_by_value();
	compute_range();
	save_cal();
}

/* Extract angle from the Message */
void can_proc_move_to_angle_msg ( sCAN* mMsg )
{
	// Angle is a 4 byte signed integer.  It is degrees * 100
	float tmp_angle 	 = extract_float_msg( mMsg->data );
	Destination.position = convert_to_value( convert_to_fixedpoint(tmp_angle) );
	Destination.speed 	 = extract_long_int( &(mMsg->data[4]) );	// [0..100%]
	motor_set_duty( Destination.speed );
}

/* Extract angle from the Message */
void can_proc_move_speed_msg ( sCAN* mMsg )
{
	// for FREE SPIN mode:
	Destination.speed = extract_long_int( mMsg->data );
}
void can_proc_max_acceleration_msg ( sCAN* mMsg )
{
}

/******************************************************************************/
/************************ OUTGOING MESSAGE PROCESSING *************************/
/******************************************************************************/
/* TRANSMITTING MESSAGE IDs:
#define ID_CALIBRATED_STOP				0x0045		// Upon request FOR DEBUG  1 or 2 in data[0]
#define ID_MOTOR_ANGLE					0x0046		// Instance goes with Transmitter
#define ID_MOTOR_VALUE					0x0047
#define ID_MOTOR_SPEED					0x0048
#define ID_MOTOR_ACCEL					0x0049
#define ID_MOTOR_STATUS					0x004A

	TRANSMITTED:
		Current Motor Angle
		Current Motor Speed (from tach or pot difference)
		Current Motor Accel (from tach or pot difference)
		Calibrated Stop 1	(POT value & angle)
		Calibrated Stop 2	(POT value & angle)
***********************************************************/
/* For informational purposes, send our travel end points. */
void can_prep_stop( sCAN* mMsg, byte mStopNumber )
{
	word PotValue;
	long int Angle;
    mMsg->id 	  = create_CAN_eid( 0x00, ID_CALIBRATED_STOP, MyInstance );
    
    if (mStopNumber==0) {
    	PotValue = EndPoint1.value;
    	Angle    = EndPoint1.angle;
    } else {
    	PotValue = EndPoint2.value;
    	Angle    = EndPoint2.angle;
    }
    
    mMsg->data[0] = mStopNumber;
	mMsg->data[1] = hi(PotValue);
	mMsg->data[2] = lo(PotValue);	

	word tmp = hi_word(Angle);
	mMsg->data[3] = hi(tmp);
	mMsg->data[4] = lo(tmp);	
	tmp = lo_word(Angle);
	mMsg->data[5] = hi(tmp);
	mMsg->data[6] = lo(tmp);	
	    
    mMsg->header.DLC = 7;
    mMsg->header.rtr = 0;
}

void can_prep_motor_angle( sCAN* mMsg )
{
	byte size = 4;
    mMsg->id = create_CAN_eid( ID_MOTOR_ANGLE, MyInstance );
	// GET CURRENT ANGLE :
	float_array Angle; 
	Angle.value = get_motor_angle();

	for (int i=0; i<size; i++)
	    mMsg->data[i] = Angle.array[i];

	// MOTOR CURRENT (Larger of left/right) (in Amps*10):
	mMsg->data[size+0] = hi(LeftCurrentSample[1]*10 );
	mMsg->data[size+1] = lo(LeftCurrentSample[1]*10 );
	mMsg->data[size+2] = hi(RightCurrentSample[1]*10);
	mMsg->data[size+3] = lo(RightCurrentSample[1]*10);
    mMsg->header.DLC = size+4;
    mMsg->header.rtr = 0;
}

void can_prep_motor_values_raw( sCAN* mMsg )
{
    mMsg->id 	  = create_CAN_eid( 0x00, ID_MOTOR_VALUE, MyInstance );
	if (isConfigured(MODE_USE_ENCODER))  {
		mMsg->data[0] = hi(EncoderCount);
		mMsg->data[1] = lo(EncoderCount);	
	} else {
		mMsg->data[0] = hi(PotSample[1]);
		mMsg->data[1] = lo(PotSample[1]);
	}
	
	// Also send Currents Raw:
	mMsg->data[2] = hi(LeftCurrentSample[1] );
	mMsg->data[3] = lo(LeftCurrentSample[1] );
	mMsg->data[4] = hi(RightCurrentSample[1]);
	mMsg->data[5] = lo(RightCurrentSample[1]);

	word Duty = MotorDutyRequest * 100;
	mMsg->data[6] = hi(Duty);
	mMsg->data[7] = lo(Duty);	
    mMsg->header.DLC = 8;
    mMsg->header.rtr = 0;
}

void can_prep_motor_speed( sCAN* mMsg, float_array mSpeed )
{
    mMsg->id 	  = create_CAN_eid( 0x00, ID_MOTOR_SPEED, MyInstance );
	if (config_byte_1 & MODE_USE_ENCODER)  {
		mMsg->data[0] = hi(EncoderSpeed);
		mMsg->data[1] = lo(EncoderSpeed);
	} else {
		for (int i=0; i<size; i++)
		    mMsg->data[i] = mSpeed.array[i];
	}
    mMsg->header.DLC = size;
    mMsg->header.rtr = 0;
}

/* size of a float IEEE single precision is usually 4 bytes */
void can_prep_motor_accel( sCAN* mMsg, float_array mAccel )
{
    mMsg->id 	  = create_CAN_eid( 0x00, ID_MOTOR_ACCEL, MyInstance );
	if (config_byte_1 & MODE_USE_ENCODER)  {    
		mMsg->data[0] = hi(EncoderAcceleration);
		mMsg->data[1] = lo(EncoderAcceleration);
	} else {
   		mMsg->data[0] = hi(PotAcceleration[1]);
   		mMsg->data[1] = lo(PotAcceleration[1]);
	}
    mMsg->header.DLC = size;
    mMsg->header.rtr = 0;
}

void can_prep_motor_status( sCAN* mMsg )
{
    mMsg->id 	  = create_CAN_eid( 0x00, ID_MOTOR_SPEED, MyInstance );
	/* Send status byte:  0=stopped, 1=forward, 2=backward, 3=in stop 1 region
							4=in stop 2 region, 5=OverTemp, 6=OverVoltage,
							8= Trouble Codes set,					*/
	mMsg->data[0] = MotorState;							
    mMsg->header.DLC = 1;
    mMsg->header.rtr = 0;
}

