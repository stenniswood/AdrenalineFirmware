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
#include "leds.h"
#include "buttons.h"
#include "analog.h"

struct sCAN tcan;


void can_file_message( sCAN* mMsg )
{
	// Analog board is mostly a transmitter!
}

/* Package up the Analog readings into CAN message 
	3 Samples per CAN message.
*/
void can_prep_analog_msg( sCAN* mMsg, uint8_t mStartIndex, uint16_t* mReadings )
{
    mMsg->id 	  = create_CAN_eid(ID_ANALOG_MEASUREMENT, MyInstance); 
    mMsg->data[0] = mStartIndex;

    mMsg->data[1] = hi(mReadings[mStartIndex]  );
    mMsg->data[2] = lo(mReadings[mStartIndex]  ); 
    
    mMsg->data[3] = hi(mReadings[mStartIndex+1]);   
    mMsg->data[4] = lo(mReadings[mStartIndex+1]);
    
    mMsg->data[5] = hi(mReadings[mStartIndex+2]);
    mMsg->data[6] = lo(mReadings[mStartIndex+2]);

    mMsg->header.DLC = 7;
    mMsg->header.rtr = 0;
}

void can_send_analog_msgs( )
{
	for (int i=0; i<8; i+=3)
	{
		can_prep_analog_msg( &tcan, i, Readings );
		can_send_msg( 0, &tcan );
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

