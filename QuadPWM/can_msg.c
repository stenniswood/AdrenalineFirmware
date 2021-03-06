/*********************************************************************
Product of Beyond Kinetics, Inc

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
********************************************************************/
#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "can_eid.h"
#include "can_instance.h"
#include "leds.h"
#include "buttons.h"
#include "spi_8500.h"
#include "propulsion.h"


/************************************************************/
/* INCOMING MESSAGE RESPONSES : 
Call back function from the CAN ISR().  
Will parse the result based on the msg id.  */
/************************************************************/
void can_file_message( sCAN* mMsg )
{
	if ( id_match( mMsg->id, create_CAN_eid(ID_PWM_CHANNEL_UPDATE, MyInstance)) )
	{
		can_parse_pwm_msg( mMsg );
	}
	if ( id_match( mMsg->id, create_CAN_eid(ID_CORR_FACTOR_UPDATE, MyInstance)) )
	{
		can_parse_cor_msg( mMsg );
	}
	if ( id_match( mMsg->id, create_CAN_eid(ID_ACCEL_XYZ, 0)) )
	{
		can_parse_tilt_msg( mMsg );
	}
	if ( id_match( mMsg->id, create_CAN_eid(ID_THRUST_UPDATE, MyInstance)) )
	{
		can_parse_thrust_msg( mMsg );
	}
		
}

/*********************************************************
 Input  :  ID
 Return : get the starting index into the PWM array
**********************************************************/
byte can_parse_pwm_msg_id ( uint16_t mID)
{
	return (mID-CAN_PWM_BASE_ID) * 4;
}	

word get_12Bit( byte* mData )
{
	word value = (mData[0] & 0x0F) << 4;
	value  += (mData[1] & 0xFF);
	return value;
}

/* Tells if the message is a PWM update message.
	Other messages are supported which specify Board ID, instance ID,
	Correction factors, etc.  These all return FALSE	*/
byte is_PWM_Msg( sCAN* mMsg )
{
	word id = get_id(mMsg->id);	
	if ((id > CAN_PWM_BASE_ID) &&
		(id < CAN_PWM_LAST_SLOT_ID) )
			return TRUE;
	return FALSE;
}

// Extract the PWM word array; Updates PWM array
void can_parse_pwm_msg ( sCAN* mMsg )
{
/* The MSG :
		Data is not packed.  Each PWM occupies full 2 bytes.  However the unused upper
		nibble of the first byte contains the index of PWM groups.
		(data[0] & 0xF0)  contains the index of PWM channel groups.  ie.  Channels:
		0..3	nFFF  0FFF  0FFF 0FFF
		4..7		8..11
		12..15		16..18		19..22		23..26
		27..30		31..34		35..38		39..42
		43..46
		47..48	nFFF 0FFF ] end.   */
	byte first_index = (mMsg->data[0] & 0xF0);
	PWM_array[first_index]   = ((mMsg->data[0]) & 0x0F) + (mMsg->data[1]);
	PWM_array[first_index+1] = ((mMsg->data[2]) & 0x0F) + (mMsg->data[3]);
	PWM_array[first_index+2] = ((mMsg->data[4]) & 0x0F) + (mMsg->data[5]);
	PWM_array[first_index+3] = ((mMsg->data[6]) & 0x0F) + (mMsg->data[7]);
}

// Extract the PWM word array; Updates PWM array
void can_parse_cor_msg ( sCAN* mMsg )
{
	byte first_index = (mMsg->data[0] & 0xF0);
	CorrectionBits[first_index]   =  (mMsg->data[0]);
	CorrectionBits[first_index+1] =  (mMsg->data[2]);
	CorrectionBits[first_index+2] =  (mMsg->data[4]);
	CorrectionBits[first_index+3] =  (mMsg->data[6]);
}

/************************************************************
  Operates on the currently selected CANPAGE MOB
  Adds a timestamp in addition to can_retrieve_data()
************************************************************/
void can_init_test_msg()
{
    msg1.id 	 = create_CAN_eid( 0x02, 0x0004, 0x01);
    msg1.data[0] = 0xA0;
    msg1.data[1] = 0x56;
    msg1.data[2] = 0xA1;
    msg1.data[3] = 0x57;
    
    msg1.data[4] = 0x12;
    msg1.data[5] = 0x34;
    msg1.data[6] = 0xEE;
    msg1.data[7] = 0xFF;
    msg1.header.DLC = 8;
    msg1.header.rtr = 0;
}


/*
	OLD WAY (had a separate message for every 6 channels)
	if ( id_match( mMsg->id , create_CAN_eid(ID_CHANNELS_0_to_6, MyInstance)) )	
	{
		can_parse_pwm_msg( mMsg );
	}
	else if ( id_match(mMsg->id , create_CAN_eid(ID_CHANNELS_7_to_13, MyInstance)) )
	{
		can_parse_pwm_msg( mMsg );
	}	
	else if ( id_match(mMsg->id , create_CAN_eid(ID_CHANNELS_14_to_20, MyInstance)) )
	{
		can_parse_pwm_msg( mMsg );
	}	
	else if ( id_match(mMsg->id , create_CAN_eid(ID_CHANNELS_21_to_27, MyInstance)) )
	{
		can_parse_pwm_msg( mMsg );
	}	
	else if ( id_match(mMsg->id , create_CAN_eid(ID_CHANNELS_28_to_34, MyInstance)) )
	{
		can_parse_pwm_msg( mMsg );
	}	
	else if ( id_match(mMsg->id , create_CAN_eid(ID_CHANNELS_35_to_41, MyInstance)) )
	{
		can_parse_pwm_msg( mMsg );
	}	
	else if ( id_match(mMsg->id , create_CAN_eid(ID_CHANNELS_42_to_48, MyInstance)) )
	{
		can_parse_pwm_msg( mMsg );
	}	*/
