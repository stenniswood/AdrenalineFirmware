/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles CAN 2.0B extended identifiers.
BK identifiers are composed of 3 parts:

Each board in the network gets a unique ID known as the instance id.
This is determined upon startup of the network.  The id's are held in 
non-volatile ram until there is a conflict (which would happen by swapping 
a module from another network;  at this time a re-initialization of every
board id is initialized (as in a new network) and the new id's are stored 
into non-volatile memory.

The complete 29 bit message consists of:	
Bits	Name			Description
-----	----			---------------------------
5 		Block_ID		(for remapping messages)
16 		Message_ID
8		Instance_ID 	because there may be more than 1 button board put into the network!
--------------------------------------------
// These create a complete ID from portions:
tID		create_CAN_eid( word mIdentifier, byte mInstance )
tID		create_CAN_eid( byte mBlock, word mIdentifier, byte mInstance	)

// These extract portions from a complete ID:
byte   get_block	(  tID mID  )
byte   get_instance	(  tID mID  )	
word   get_id		(  tID mID  )

// These compare portions of the ID
bool   match		( tID m1, tID m2 )
bool   block_match	( tID m1, tID m2 )
bool   id_match		( tID m1, tID m2 )
bool   instance_match( tID m1, tID m2 )
-----------------------------------------------------
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "leds.h"

/*************************************************
create_CAN_eid()
INPUTS:
	Message_ID
	Instance_ID
OUTPUT:
	return - a complete CAN 2.0B id (4 bytes)
**************************************************/
tID		create_CAN_eid( word mIdentifier, byte mInstance )
{
	tID id;
	id.group.block 	   = (0x00);
	id.group.id 	   = (mIdentifier);
	id.group.instance  = (mInstance);
	return id;
}

tID		create_CAN_eid( byte mBlock, word mIdentifier, byte mInstance	)
{
	tID id;
	id.group.block		= (mBlock);
	id.group.id			= (mIdentifier);
	id.group.instance	= (mInstance);
	return id;
}


/*bool  block_match	( tID m1, tID m2 );
bool  id_match   	( tID m1, tID m2 );
bool  instance_match( tID m1, tID m2 ); */

//===================== ACCESSOR FUNCTIONS: ==============================
byte   get_block(  tID mID  )				
{
	return (mID.group.block);
//	return (mID & 0xFF000000) >> 24;
}
byte   get_instance(  tID mID  )				
{
	return (mID.group.id);
//	return (mID & 0x000000FF);
}
word   get_id(  tID mID  )	
{
	return (mID.group.instance);
//	return (mID & 0x00FFFF00) >> 8;
}

//=================== MATCHING FUNCTIONS: =======================
bool   match( tID m1, tID m2 )
{
	return (m1.full_id == m2.full_id);
}

bool   block_match( tID m1, tID m2 )
{
	return (m1.group.block == m2.group.block);
//	return (get_block(m1) == get_block(m2));
}
bool   id_match( tID m1, tID m2 )
{
	return (m1.group.id == m2.group.id);
}
bool   instance_match( tID m1, byte m2 )
{
	return (m1.group.instance == m2);
//	return (get_instance(m1) == get_instance(m2));
}

