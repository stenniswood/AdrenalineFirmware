/*********************************************************************
Product of Beyond Kinetics, Inc

This code handles can messages specific to the button board

A) Constructs a message with the current key state
B) Software Firmware Version Message
C) Board ID to the network

The complete 29 bit message consists of:	
21	MESSAGE_ID

8	INSTANCE_ID because there may be more than 1 button board put into the network
-----------------------
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
#include "leds.h"
#include "buttons.h"
#include "can_id_list.h"
#include "can_instance.h"
#include "can_eid.h"
#include "can_board_msg.h"


/************************************************************
INCOMING CAN MESSAGE Callback : 
	Call back function from the CAN ISR().  

 Since each board has a unique 8 bit identifier (Instance number)
   The last 8 bits of the LED message identifiers below will 
   specify the board which board the LED is intended for.  Since this
   filtering will likely be in hardware, we don't need to check this
   in the CAN software callback function.
************************************************************/
void can_file_message( sCAN* mMsg )
{
}

//////////////////////////////////////////////////////////////////////////////
////////////////////// OUT GOING MESSAGES ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/************************************************************
 Encoding is : 
		 [0] upper nibble is front row
		 [0] lower        is top row
		 [1] upper nibble is 2nd row
	 	 [1] lower        is 3rd row
		 [2] upper nibble is 4th row		 
		 [2] lower nibble is 4th row		 
		 [3] upper nibble is 4th row
		 [3] lower nibble is 4th row
************************************************************/
void can_prep_button_roster_msg( sCAN* mMsg, union uKeyRoster mkeypad )
{
	static byte count = 0;
    mMsg->id 	  = create_CAN_eid( ID_BUTTON_ROSTER, MyInstance );
    mMsg->data[0] = mkeypad.array[0];	 // Upper Nibble is count of pressed buttons.  Lower nibble is the front edge buttons.
    mMsg->data[1] = mkeypad.array[1]; 	 // 
    mMsg->data[2] = mkeypad.array[2];    // 
    mMsg->data[3] = count++;	 		 // 
    mMsg->header.DLC = 4;
    mMsg->header.rtr = 0;
}

/************************************************************
	Each button has a number between [0..15]
INPUT:	mKeys
		mNumberKeysPressed
		 [0] NumberKeysPressed
		 [1] First Key id 
		 [2] Second Key id (if 2 pressed)
		 [...] Last key id
		 [button msg counter] to make each can msg unique.
************************************************************/
byte counter = 0;
void can_prep_button_pressed_msg( sCAN* mMsg )
{
    mMsg->id 	  = create_CAN_eid( ID_BUTTON_PRESSED, MyInstance );
	if (NumberKeysPressed > 7)
		NumberKeysPressed = 7;

    mMsg->data[0] = NumberKeysPressed;
    for (int i=0; i<NumberKeysPressed; i++)
	    mMsg->data[i+1] = keys_down[i];

	mMsg->data[NumberKeysPressed+1] = counter++;
    mMsg->header.DLC = 2+NumberKeysPressed;
    mMsg->header.rtr = 0;
}

/************************************************************
  Operates on the currently selected CANPAGE MOB
  Adds a timestamp in addition to can_retrieve_data()
************************************************************/
void can_init_test_msg(byte* tmp)
{
	static word count = 0;
	count++;
    msg1.id 	 = create_CAN_eid( 0x02, 0x0004, 0x01);
    msg1.data[0] = tmp[0];
    msg1.data[1] = tmp[1];
    msg1.data[2] = tmp[2];
    msg1.data[3] = tmp[3];
    msg1.data[4] = tmp[4];
    
    msg1.data[5] = PORTC;
    msg1.data[6] = SWITCH_DDR;
    msg1.data[7] = 0xFF;
    msg1.header.DLC = 8;
    msg1.header.rtr = 0;
}
