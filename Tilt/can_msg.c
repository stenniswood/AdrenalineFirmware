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
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "bk_system_defs.h"
#include "can.h"
#include "can_msg.h"
#include "leds.h"
#include "can_eid.h"
#include "can_id_list.h"
#include "can_instance.h"


struct sCAN tcan;
/*struct sCAN accel_can;
struct sCAN gyro_can ;
struct sCAN magnet_can; */

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

void can_prep_generic_XYZ_msg_msb( sCAN* mMsg, union uXYZ* mData )
{
	// mMsg->id should be set before calling!
	mMsg->data[0] = mData->array[0];		// hi for atmel goes to the 2nd byte (little endian)
	mMsg->data[1] = mData->array[1];		// We really should go thru the array part of the union
	mMsg->data[2] = mData->array[2];		// I'd change it, but not sure what else this would effect
	mMsg->data[3] = mData->array[3];		// May cause a problem by other callers.  So Use new function 
	mMsg->data[4] = mData->array[4];		// can_prep_generic_XYZ_msg() 
	mMsg->data[5] = mData->array[5];
    mMsg->header.DLC = 6;
    mMsg->header.rtr = 0;
}

void can_prep_generic_XYZ_msg( sCAN* mMsg, union uXYZ* mData )
{
	// mMsg->id should be set before calling!
	mMsg->data[0] = hi(mData->value[0]);		// hi for atmel goes to the 2nd byte (little endian)
	mMsg->data[1] = lo(mData->value[0]);		// We really should go thru the array part of the union
	mMsg->data[2] = hi(mData->value[1]);		// I'd change it, but not sure what else this would effect
	mMsg->data[3] = lo(mData->value[1]);		// May cause a problem by other callers.  So Use new function 
	mMsg->data[4] = hi(mData->value[2]);		// can_prep_generic_XYZ_msg() 
	mMsg->data[5] = lo(mData->value[2]);
    mMsg->header.DLC = 6;
    mMsg->header.rtr = 0;
}

/* RAW DEVICE - CAN MESSAGES */
void can_prep_accel_msg		( sCAN* mMsg, union uXYZ* mData )
{
	mMsg->id = create_CAN_eid(ID_ACCEL_XYZ, MyInstance);
	can_prep_generic_XYZ_msg( mMsg, mData );
}
void can_prep_gyro_msg		( sCAN* mMsg, union uXYZ* mData )
{
	mMsg->id = create_CAN_eid(ID_GYRO_XYZ, MyInstance);
	can_prep_generic_XYZ_msg( mMsg, mData );
}
void can_prep_magnet_msg	( sCAN* mMsg, union uXYZ* mData )
{
	mMsg->id = create_CAN_eid(ID_MAGNET_XYZ, MyInstance);
	can_prep_generic_XYZ_msg_msb( mMsg, mData );
}

/* PROCESSED DATA - CAN MESSAGES */

// ***** POSITION *****  (TRANSLATIONAL & ANGULAR)
void can_prep_total_position_msg( sCAN* mMsg, union uXYZ* mData )
{
	mMsg->id = create_CAN_eid(ID_POSITION_XYZ, MyInstance);
	can_prep_generic_XYZ_msg( mMsg, mData );
}
void can_prep_total_angular_position_msg( sCAN* mMsg, union uXYZ* mData )
{
	mMsg->id = create_CAN_eid(ID_POSITION_ABC, MyInstance);
	can_prep_generic_XYZ_msg( mMsg, mData );
}

// ***** VELOCITY ***** (TRANSLATIONAL & ANGULAR)
void can_prep_total_velocity_msg( sCAN* mMsg, union uXYZ* mData )
{
	mMsg->id = create_CAN_eid(ID_VELOCITY_XYZ, MyInstance);
	can_prep_generic_XYZ_msg( mMsg, mData );	
}
void can_prep_total_angular_velocity_msg( sCAN* mMsg, union uXYZ* mData )
{
	mMsg->id = create_CAN_eid(ID_VELOCITY_ABC, MyInstance);
	can_prep_generic_XYZ_msg( mMsg, mData );
}

// ***** ACCELERATION *****  (TRANSLATIONAL & ANGULAR)
// these are not supported currently.
void can_prep_total_acceleration_msg( sCAN* mMsg, union uXYZ* mData )
{
	mMsg->id = create_CAN_eid(ID_ACCELERATION_XYZ, MyInstance);
	can_prep_generic_XYZ_msg( mMsg, mData );
}
void can_prep_total_angular_acceleration_msg( sCAN* mMsg, union uXYZ* mData )
{
	mMsg->id = create_CAN_eid(ID_ACCELERATION_ABC, MyInstance);
	can_prep_generic_XYZ_msg( mMsg, mData );
}


//void can_prep_matrix_msg( sCAN* mMsg, union uXYZ mData ) { }

/************************************************************
  Operates on the currently selected CANPAGE MOB
  Adds a timestamp in addition to can_retrieve_data()
***********************************************************
void can_init_test_msg()
{
    msg1.id 	 = create_CAN_eid( 0x00, 0x0092, MyInstance );
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
}*/


