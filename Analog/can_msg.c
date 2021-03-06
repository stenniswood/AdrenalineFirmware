/*********************************************************************
This code handles can messages specific to the button board

A) Constructs a message with the current key state
B) Software Firmware Version Message
C) Board ID to the network

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
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "can_instance.h"
#include "can_eid.h"
#include "can_id_list.h"

#include "calibrations.h"
#include "configuration.h"
#include "leds.h"
#include "analog.h"

struct sCAN tcan;


void can_file_message( sCAN* mMsg )
{
	// Analog board is mostly a transmitter!
}

/* Package up the Analog readings into CAN message 
	1 Channel per CAN message.
	Optionally may have the derivative of the channel.
*/
void can_prep_analog_msg( sCAN* mMsg, uint16_t mReading, byte mMeasurementIndex )
{
    mMsg->id 	  = create_CAN_eid(ID_ANALOG_MEASUREMENT, MyInstance); 
    mMsg->data[0] = mMeasurementIndex;
    mMsg->data[1] = hi(mReading);
    mMsg->data[2] = lo(mReading);     
    mMsg->header.DLC = 3;
    mMsg->header.rtr = 0;
    //mMsg->data[0] = mCounter;   
}

void can_prep_analog_derivative_msg( sCAN* mMsg, 
							byte     mMeasurementIndex,
							uint16_t mReading, 
							uint16_t mReadingPrev )
{
    mMsg->id 	  = create_CAN_eid(ID_ANALOG_MEASUREMENT, MyInstance); 
    mMsg->data[0] = mMeasurementIndex;
    mMsg->data[1] = hi(mReading  );
    mMsg->data[2] = lo(mReading  );     
    short diff = (mReading - mReadingPrev);
    mMsg->data[3] = hi(diff);   
    mMsg->data[4] = lo(diff);   
    mMsg->header.DLC = 5;
    mMsg->header.rtr = 0;
}

/* 4 Chips;  16 channels each.  = 64 channels total for Analog board. 
	64 channels * 2 bytes package = 128 bytes 
	/ 8 = 16 CAN msgs minimum.
		DEPRECATED!
*/
void can_send_analog_msgs( )
{
/*	byte configd;
	byte test;
	for (int chip=0; chip<4; chip++)
	{
		configd = isConfigured( (1<<chip) );
		if ( configd ) 
			for (int chan=3; chan<9; chan++)
			{
				delay(50000);
				test = (MODE_SEND_DERIVATIVES_1<<chip);
				if ( isConfigured(test) )
				{
					can_prep_analog_derivative_msg( &tcan, 
									chan,
									Readings[chip*16+chan],
									ReadingsPrev[chip*16+chan] );
					can_send_msg( 0, &tcan );
				}
				else 
				{
					can_prep_analog_msg ( &tcan, Readings[chip*16+chan], chan );
					//can_send_msg_no_wait( 0, &tcan );
					can_send_msg( 0, &tcan );
				}
			}
	} */
}

/* Sends Just 1 Analog channel */
void can_send_one(byte mChip, byte mchan)
{
	byte index = 0;
	byte test = (MODE_SEND_DERIVATIVES_1<<mChip);
	if ( isConfigured(test) )
	{
		can_prep_analog_derivative_msg( &tcan, mchan,
						Readings[mChip*16+mchan],
						ReadingsPrev[mChip*16+mchan] );
		can_send_msg( 0, &tcan );
	}
	else 
	{
		index = (mChip<<4) | mchan;	
		can_prep_analog_msg ( &tcan, Readings[mChip*16+mchan], index );
		can_send_msg( 0, &tcan );
	}
}

static short ReportCounter = 1;		// when zero sends update

void can_send_timeslice()
{
	byte   configd;
	static byte chip = 0;
	byte   channel = 0;

	if (isConfigured(0x0F)==0)		// No chips enabled...
		return;

	if (isReportingEnabled())
	{
		ReportCounter--;
		if (ReportCounter<=0)			// Time To Report ? 
		{ 
			ReportCounter = getReportRate();	// Refill
			led_on(2);

			// TRANSMIT ALL CHANNELS FOR A GIVEN CHIP:
			byte channels = get_num_channels(chip);
			do {
				can_send_one( chip, channel );
				channel++;
			} while (channel < channels);
		
			// Prepare for Next Chip!
			do {
				chip++;
				if (chip>3) chip = 0;
				configd = isConfigured( 1<<chip );
			} while ( !configd );

			led_off(2);
		}
	}
}

void can_init_test_msg()
{
    msg1.id 	 = create_CAN_eid( ID_ANALOG_MEASUREMENT, MyInstance );
    msg1.data[0] = 0xA0;
    msg1.data[1] = 0x55;
    msg1.data[2] = 0xA1;
    msg1.data[3] = 0x55;    
    msg1.header.DLC = 4;
    msg1.header.rtr = 0;
}

