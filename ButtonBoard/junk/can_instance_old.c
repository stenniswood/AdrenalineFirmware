#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "can_eid.h"
#include "can_id_list.h"
#include "can_instance.h"
#include "leds.h"
#include "eeprom.h"

/* Strategy:
		We begin counting from 0 (or down from 255)
		If we receive a message with our attempted id, then bump and try sending again.
		If we receive a count above our attempted id, means we were listening/somehow missed a message.

	Once the Instance ID is found for each board, this must be saved into EEPROM 
	so that future sessions can refer to the knee motor with the same id.
	Otherwise we'll have randomized boards and motor movements belonging to knee will go
	to the hip, etc.			
*/

// NEEDS TO BE SAVED INTO EEPROM!
byte MyInstance = 0x00;
byte Confirmed  = 0x00;			// MyInstance was approved on previous boot.

word pick_random_number()
{
	return TCNT1;
}

/***************************************************************
The ID_RESERVE_INSTANCE_REQUEST msg uses a random number for its 
"instance" since the instance numbers have not been established 
for the boards yet - the can priority mechanism would have no preference
if they each used sequential instance ids.  ie we would have multiple
boards claiming "i want to request instance 0".  With a random number
each board has a priority which can be easily resolved.  Once the 
requests are granted, from then on priority is not a problem if it's sequential
The first data member of the msg contains the id we wish to reserve.

***************************************************************/
void can_prep_instance_request( sCAN* mMsg )
{
	word 	rand_instance = pick_random_number();
    mMsg->id 	  = create_CAN_eid( 0x02, ID_RESERVE_INSTANCE_REQUEST, rand_instance );
	mMsg->data[0] = MyInstance;
    mMsg->header.DLC    = 1;
    mMsg->header.rtr    = 0;
    can_send_msg( 0, mMsg );
}

/************************************************************
 We received a request.
 ***********************************************************/
void can_process_instance_request( sCAN* mMsg )
{
	byte instance = mMsg->data[0];
	if (instance == MyInstance)
	{  
		MyInstance = instance+1;
		// Terminate any pending transmit.
		
		// Rerequest instance 
		can_prep_instance_request( &msg2 );
	}
}


void save_instance_number()
{
    eeprom_write_byte( (unsigned char*)INSTANCE_EEPROM_ADDRESS, MyInstance );
}

void read_instance_number()
{
    MyInstance = eeprom_read_byte( (unsigned char*)INSTANCE_EEPROM_ADDRESS );
}

