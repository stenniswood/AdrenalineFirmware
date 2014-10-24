#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
//#include "can_id.h"
#include "leds.h"

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


bool  block_match	( tID m1, tID m2 );
bool  id_match   	( tID m1, tID m2 );
bool  instance_match( tID m1, tID m2 );

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
//	return (m1 == m2);
}
bool   block_match( tID m1, tID m2 )
{
	return (m1.group.block == m2.group.block);
//	return (get_block(m1) == get_block(m2));
}
bool   id_match( tID m1, tID m2 )
{
	return (m1.group.id == m2.group.id);
//	return (get_id(m1) == get_id(m2));
}
bool   instance_match( tID m1, tID m2 )
{
	return (m1.group.instance == m2.group.instance);
//	return (get_instance(m1) == get_instance(m2));
}



