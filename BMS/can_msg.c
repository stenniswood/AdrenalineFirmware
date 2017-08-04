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
#include "can_board_msg.h"

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
word extract_word_be( byte* mData )
{
	long int retval = 0;
	retval |= (mData[0]<<8);
	retval |= mData[1];
	return retval;
}
word extract_word_le( byte* mData )
{
	long int retval = 0;
	retval |= mData[0];
	retval |= (mData[1]<<8 );
	return retval;
}

long int extract_long_int_be( byte* mData)
{
	long int retval = 0;
	retval |= (((unsigned long)mData[0])<<24);
	retval |= (((unsigned long)mData[1])<<16);
	retval |= (((unsigned long)mData[2])<<8);
	retval |= (((unsigned long)mData[3]));
	return retval;
}
long int extract_long_int_le( byte* mData)
{
	long int retval = 0;
	retval |= mData[0];
	retval |= (mData[1]<<8 );
	retval |= ((long)mData[2])<<16;
	retval |= ((long)mData[3]<<24);
	return retval;
}

/******************************************************************************/
/************************ INCOMING MESSAGE PROCESSING *************************/
/******************************************************************************/
void can_file_message( sCAN* mMsg )
{
	if ( match( mMsg->id, 	create_CAN_eid(ID_MARK_MOTOR_STOP, MyInstance)) )
	{
	}
	else if ( (isConfigured(MODE_TILT_RESPONDER)) && 
			  (id_match(mMsg->id, create_CAN_eid(ID_ACCEL_XYZ, 0)) ) )
	{
	
	}
}

/* Extract angle from the Message */
#define KConst 0.25
void can_proc_tilt_msg( sCAN* mMsg )
{
}

/* Extract angle from the Message */
void can_proc_set_stop_msg( sCAN* mMsg )
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
		Average Current value.  Cumulative (couloumb counter) current.
***********************************************************/
/* For informational purposes, send current of a power supply.
	mSupplyIndex  [0..3] which 5V supply to send status of.
*/
void can_prep_currents( sCAN* mMsg, byte mSupplyIndex )
{
	byte size = sizeof(float);
    mMsg->id = create_CAN_eid( ID_BMS_CURRENT_1+mSupplyIndex, MyInstance );

	// Send CURRENT :	(Fixed point)
	float_array Angle; 	
	Angle.value = CurrentSampleAvg[mSupplyIndex];
	for (int i=0; i<size; i++)
	    mMsg->data[i] = Angle.array[i];

	// Send Coloumbs :	(Fixed point)
	Angle.value = Cumulative[mSupplyIndex];
	for (int i=size; i<size+size; i++)
	    mMsg->data[i] = Angle.array[i];

    mMsg->header.DLC = size+2;
    mMsg->header.rtr = 0;
}
