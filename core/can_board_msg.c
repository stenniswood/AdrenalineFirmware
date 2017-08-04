/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles can messages for board identification 
			( model/type, hardware/software version, serial number, etc)
and 
		System wide LED Indication messages
			Some could use to make visually appeasing LED display on the robot stack.

The complete 29 bit message consists of:	
Bits	Name			Description
-----	----			---------------------------
5 		Block			(for remapping messages)
16 		MESSAGE_ID
8		INSTANCE_ID 	because there may be more than 1 button board put into the network
------------------------
INCOMING MESSAGES:
	ID_BOARD_PRESENCE_REQUEST		0x0010		(0 Instance)
	ID_SYSTEM_LED_REQUEST						(MyInstance specific)

OUTGOING MESSAGES:
	ID_BOARD_PRESENCE_BROADCAST		0x0011		
	ID_BOARD_REVISION				0x0012		(MyInstance tagged)
	ID_BOARD_SERIAL_NUMBER			0x0013		(MyInstance tagged)
	ID_BOARD_DESCRIPTION			0x0014		(MyInstance tagged)
--------------------------------------------
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <string.h>
#include "bk_system_defs.h"
#include "pgmspace.h"
#include "inttypes.h"
#include "pin_definitions.h"
#include "can_id_list.h"
#include "can.h"
#include "can_eid.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "can_msg.h"
#include "can_buff.h"
#include "configuration.h"
#include "calibrations.h"
#include "leds.h"


// FOR DEBUG!:
extern void protected_led_on(byte mindex);
extern void protected_led_off(byte mindex);

/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
// DESCRIPTION:
byte BoardStatus 		= 0x00;			// Need to update this from DTCLogger class.
byte MappedBlock 		= 0x00;
byte Starting_Index 	= 0;			// ?
byte System_Send_Status = 0;			// Use is OS_Dispatch() to send the requested message.
byte ShutDownState 		= NO_SHUTDOWN_PENDING;


const char BoardDescription[] PROGMEM ="Drives DC motor";
//////////////////////////////////////////////////////////////////////////////
////////////////////// INCOMING MESSAGES /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void (*config_call_back)(byte mByteChanged) = NULL;		// Call back function
void set_configure_callback( void (*mCallback)(byte mByteChanged) )
{
	// ByteChanged [1..4] is the config byte which changed.
	config_call_back = mCallback;
}

byte Board_presence_response_type = 0;
int  Board_presence_delay 		  = -1;	// nothing pending

/**********************************************************************
INCOMING CAN MESSAGE Callback : 
	Call back function from the CAN ISR()

 Since each board has a unique 8 bit identifier (Instance number)
   The last 8 bits of the LED message identifiers below will 
   specify the board which board the LED is intended for.  Since this
   filtering will likely be in hardware, we don't need to check this
   in the CAN software callback function.
**********************************************************************/
BOOL can_board_msg_responder( sCAN* mMsg )
{
	if ( match( mMsg->id, create_CAN_eid			(ID_SYSTEM_LED_REQUEST, MyInstance)) ||
	     match( mMsg->id, create_CAN_eid			(ID_SYSTEM_LED_REQUEST, 0xFF)) )
	{
		can_parse_led_request( mMsg );				// in leds.c 
		return TRUE;
	}
	else if ( id_match( mMsg->id, create_CAN_eid	(ID_BOARD_PRESENCE_REQUEST, 0)) )
	{
		/* Ah we have problems getting all of these sometimes because...
			There's contention.
			Need to delay by the instance number of timeslices.
			See can_board_timeslice_100ms() below.
		*/
		//protected_led_on( 3 );
		System_Send_Status			 |=0x20;
		Board_presence_response_type = mMsg->data[0];
		Board_presence_delay 		 = MyInstance;
		return TRUE;
	}
	else if ( match( mMsg->id, create_CAN_eid		(ID_SYSTEM_CONFIGURE, MyInstance)) )
	{
		if ( (mMsg->data[0] & 0x10) == 0x10)		// Read Config command
		{
			can_prep_configure( &msg1 );
			System_Send_Status = 1;
		} else { 
			can_parse_configure_request( mMsg );	// in configuration.c
			System_Send_Status = 8;					// Save configuration!
		}
		// May want a callback here as well.  Some modules need to do things
		// as a consequence - ie accelerometers needs to send a special code
		if (config_call_back != NULL)
			config_call_back( mMsg->data[0] & 0x0F );
		return TRUE;
	}
	else if ( match( mMsg->id, create_CAN_eid	(ID_SYSTEM_SHUT_DOWN, MyInstance)) ||
		      match( mMsg->id, create_CAN_eid	(ID_SYSTEM_SHUT_DOWN, 0xFF)) )
	{
		System_Send_Status=8;
		//ShutDownState = SHUTDOWN_PENDING;
		return TRUE;
	}
	else if ( match( mMsg->id, create_CAN_eid	(ID_INSTANCE_ASSIGN, MyInstance)) )
	{
		Confirmed  = CLAIMED;
		MyInstance = mMsg->data[0];
		System_Send_Status |= 0x08;
		//save_configuration();					// maybe shouldn't do here in ISR.
   		can_remove_instance_from_filter( 1 );	// move before MyInstance
		can_add_instance_to_filter     ( 1, MyInstance );
		return TRUE;
	}
	else if (match( mMsg->id, create_CAN_eid	(ID_INSTANCE_RESTART, MyInstance)) || 
	         match( mMsg->id, create_CAN_eid	(ID_INSTANCE_RESTART, 0xFF)) )
	{
		if (mMsg->header.DLC == 2)
		{
			word passcode = (mMsg->data[0]<<8) | (mMsg->data[1]);		
			byte match  = (passcode == 0x1234);
			/* DONT CALL THIS: make_system_new_board( passcode );  We do the sequencing immediately
			   and therefore the NewBoard never has to (and should not) ever get changed. 
			   As it's name implies, it is only once per board - after the manufacturer. */
			if (match)
			{			
				init_path 	  = 0x55;
				init_complete = FALSE;
				Confirmed     = NOT_CLAIMED;
				can_instance_init();		// This fires off the sequencing right away.
											// no wait till next boot.  This works well!
			}
		}
		return TRUE;
	}	
	else if ( id_match( mMsg->id, create_CAN_eid	(ID_INSTANCE_CLAIM, 0)) )
	{
		// Abort any ID_INSTANCE_CLAIM, TX pending.
		// unless it already was sucessfully transmitted.
		//  Do not use : get_instance(mMsg->id) because during the sequence, the 
		//  id is the random delay number.  The actual id is in the message!
		byte instance = mMsg->data[0];
		if (instance == MyInstance) 
		{
			if (Confirmed == CLAIM_PENDING)
			{
					// This code is part of the CAN ISR().  So the CANPAGE is already
					// directed at the Receive buffer.  We want to terminate the Transmit
					// which is on a different MOB.
					// ABORT TRANSMIT MOB FOR ID_INSTANCE_CLAIM :
					int8_t savecanpage;
					savecanpage = CANPAGE;         		// Save current MOB
					CANPAGE = (INSTANCE_TX_MOB<<4);     // Selects MOB with highest priority interrupt
					Can_mob_abort();					// 
					CANPAGE = savecanpage;      		// Restore original MOB   				
	
					Confirmed = NOT_CLAIMED; 			// 
					can_process_instance_request( mMsg );	// bumps the MyInstance	
					
					// MyInstance updated in process function below.
			} else if (Confirmed == NOT_CLAIMED)
				can_process_instance_request( mMsg );	// bumps the MyInstance
		}
		return TRUE;
	}
	//MotorState = MOTOR_REQUEST_PENDING;
	return FALSE;
}


/***************************************************************
 A system LED request message is intended for this board.  
 ID_SYSTEM_LED_REQUEST
 
INPUT:
	data[0] Upper nibble contains the desired action [0,1]
			0 = release system control of the led indicators (to whatever functionality the specific module uses them for)
			1 = override specific purpose to show the system requested pattern.
	data[0] Lower nibble
			contains the 4 bit pattern.  If the board only has 3 leds, then they 
			will use the lowest 3 bits of this nibble.
**************************************************************/
void can_board_presence_reply(  )
{
 	switch(Board_presence_response_type)
	{
	case 0 :can_prep_board_presence_msg	( &msg2    );
			can_send_msg_no_wait		( 2, &msg2 );
			break;
	case 1 :can_prep_board_revision_msg	( &msg2    );
			can_send_msg_no_wait		( 2, &msg2 );
			break;
	case 2 :can_prep_serial_number_msg 	( &msg2    );		
			can_send_msg_no_wait		( 2, &msg2 );
			break;
	case 3 :can_prep_board_description_msg	( &msg2    );
			can_send_msg_no_wait			( 2, &msg2 );
			break;
	default: break;
	}
// This caused problems with interrupts.  triggers interrupt within interrupt.	
//	can_send_msg_no_wait( 0, &msg2 );
}

/* Sends the queued message 
	This actually is called every 20ms.
*/
void can_board_timeslice()
{	
	if ((System_Send_Status&0x01)==1) {			// Send channel 1 msg !
		can_send_msg_no_wait( 0, &msg1 );
		System_Send_Status &= (~0x01);
	}
	if ((System_Send_Status&0x02)==2) {			// Send channel 2 msg !
		can_send_msg_no_wait( 2, &msg2 );
		System_Send_Status &= (~0x02);
	}
	if ((System_Send_Status&0x04)==4) {			// Reread Configuration & send
		cli();
		// RUIN VALUES TO PROVE THE READ WORKS! no, it works.
		//MyInstance=0xAA;  Confirmed=0xAA;  sys_config_byte=0xAA;		
		read_configuration();
		can_prep_instance_request( &msg2, MyInstance );
		can_send_msg_no_wait( 0, &msg2 );
		System_Send_Status &= (~0x04);		
		sei();
	}
	if ((System_Send_Status&0x08)==0x08) {		// Save Configuration 
		cli();
		save_configuration();
		led_on(4);
		System_Send_Status &= (~0x08);
		sei();
	}
	if ((System_Send_Status&0x10)==0x10) {		// Save all calibration & send
		cli(); 
		save_cal();
		System_Send_Status &= (~0x10);
		sei();
	}
	if ((System_Send_Status & 0x20)==0x20) {	// Board presence:
		// To prevent collisions between boards, delay by the instance id
		// assigned to this board.  (Should be unique on the network).		
		if ((Board_presence_delay--)==0)
		{
			cli();
			can_board_presence_reply( );
			System_Send_Status &= (~0x20);
			sei();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
////////////////////// OUT GOING MESSAGES ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/***************************************************************************
Board Presence Identifies it's board Model, Block, Instance, & Status (1 byte) 
 The blocks may be moved around to prevent conflicting CAN ids.  This "auto-
 arrangement" scheme will be worked out later.

The Model should be #defined in the "pin_definitions.h"
****************************************************************************/
void can_prep_board_presence_msg( sCAN* mMsg )
{
	mMsg->id  = create_CAN_eid( ID_BOARD_PRESENCE_BROADCAST, MyInstance );
    mMsg->data[0] = BOARD_MODEL;
    mMsg->data[1] = MyInstance;
    mMsg->data[2] = BoardStatus;
    mMsg->data[3] = MappedBlock;
    mMsg->header.DLC	= 4;
    mMsg->header.rtr    = 0;
}

void can_prep_board_revision_msg( sCAN* mMsg )
{
	mMsg->id  = create_CAN_eid( ID_BOARD_REVISION, MyInstance );
	mMsg->data[0] = BOARD_MODEL;
    mMsg->data[1] = HARDWARE_REVISION;
    mMsg->data[2] = SOFTWARE_MAJOR;
    mMsg->data[3] = SOFTWARE_MINOR;
    mMsg->data[4] = lo(MANUFACTURER);
    mMsg->data[5] = hi(MANUFACTURER);
    mMsg->header.DLC 	= 6;
    mMsg->header.rtr    = 0;
}

void can_prep_serial_number_msg( sCAN* mMsg )
{
	mMsg->id      = create_CAN_eid( ID_BOARD_SERIAL_NUMBER, MyInstance );
	mMsg->data[0] = BOARD_MODEL;
    mMsg->data[1] = (SerialNumber & 0xFF000000) >> 24;	// MSB first
    mMsg->data[2] = (SerialNumber & 0x00FF0000) >> 16; 
    mMsg->data[3] = (SerialNumber & 0x0000FF00) >>  8; 
    mMsg->data[4] = (SerialNumber & 0x000000FF) >>  0;	// LSB 
    mMsg->header.DLC = 5;
    mMsg->header.rtr = 0;
}

void can_prep_board_description_msg( sCAN* mMsg )
{
	byte chars_sent = 0;
	mMsg->id  = create_CAN_eid( ID_BOARD_DESCRIPTION, MyInstance );
    mMsg->data[0] = Starting_Index;
    byte length = min(8, strlen(BoardDescription));
    
   	for (int i=0; i<length; i++, chars_sent++)
	    mMsg->data[i+1] = pgm_read_byte(BoardDescription+Starting_Index+i);

    mMsg->header.DLC = chars_sent+1;
    mMsg->header.rtr = 0;
}

void can_prep_configure( sCAN* mMsg )
{
	mMsg->id  = create_CAN_eid( ID_SYSTEM_CONFIGURE_REPORT, MyInstance );
    mMsg->data[0] = config_byte_1;
    mMsg->data[1] = config_byte_2;
    mMsg->data[2] = config_byte_3;
    mMsg->data[3] = config_byte_4;
    mMsg->header.DLC	= 4;
    mMsg->header.rtr    = 0;
}


						