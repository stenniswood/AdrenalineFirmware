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
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "leds.h"
#include "buttons.h"

#define DEVICE_ID	0x01000000

// FIRMWARE (Major.Minor)  (2.1)
#define MAJOR 0x01
#define MINOR 0x00

#define MANUFACTURER 0x0001				// Beyond Kinetics is Manufacturer #1 !



void can_prep_board_id_msg( sCAN* mMsg )
{
	mMsg->id = 0x0003;
    mMsg->data[0] = (DEVICE_ID & 0xFF000000) >> 24;	// MSB first
    mMsg->data[1] = (DEVICE_ID & 0x00FF0000) >> 16;
    mMsg->data[2] = (DEVICE_ID & 0x0000FF00) >>  8;
    mMsg->data[3] = (DEVICE_ID & 0x000000FF) >>  0;	// LSB 
    
    mMsg->data[4] = MAJOR;
    mMsg->data[5] = MINOR;
    mMsg->data[6] = lo(MANUFACTURER);
    mMsg->data[7] = hi(MANUFACTURER);
    mMsg->header.Length = 8;
    mMsg->header.rtr    = 0;
}

void can_prep_button_pressed_msg( sCAN* mMsg, char mKeyCharacter, byte mRow, byte mColumn )
{
    mMsg->id 	  = 0x0004;
    mMsg->data[0] = mKeyCharacter;
    mMsg->data[1] = mRow;    
    mMsg->data[2] = mColumn;
    mMsg->header.Length = 3;
    mMsg->header.rtr = 0;
}

/* Encoding is :

*/
void can_prep_button_roster_msg( sCAN* mMsg, union uKeyRoster mkeypad )
{
    mMsg->id 	  = 0x0005;
    mMsg->data[0] = mkeypad.array[0];	 // Upper Nibble is count of pressed buttons.  Lower nibble is the front edge buttons.
    mMsg->data[1] = mkeypad.array[1]; 	 // 
    mMsg->data[2] = mkeypad.array[2];    // 
    mMsg->data[3] = mkeypad.array[3];	 //
    mMsg->header.Length = 4;
    mMsg->header.rtr = 0;
}



