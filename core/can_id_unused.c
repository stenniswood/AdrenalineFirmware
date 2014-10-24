/*********************************************************************
Product of Beyond Kinetics, Inc
This file puts together various other files:  
	pot.c calibrations.c encoder.c can_msg.c 
-------------------------------------------------------------------
DATE 	:  10/20/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "can_id.h"
#include "leds.h"

//===================== CONSTRUCTORS: ==============================
CAN_id::CAN_id( word mID, byte mInstance	)
: block(0x00),
identifier( mID ),
channel( mInstance )
{
	
}

CAN_id::CAN_id( byte mBlock, word mID, byte mInstance	)
{

}


//===================== ACCESSOR FUNCTIONS: ==============================
void  CAN_id::set_block( byte mBlock )		// a grouping of ids (5 bits only)
{
	block = mBlock;
}
byte  CAN_id::get_block(  )				
{
	return block;
}

void  CAN_id::set_channel( byte mChannel )	// set channel (8 bit) instance id
{
	channel = mChannel;
}
byte  CAN_id::get_channel(  )				
{
	return channel;
}

void  CAN_id::set_id( byte mIdentifier )		// Message Identifier (16 bit)
{
	identifier = mChannel;
}
word  CAN_id::get_id(  )					// 
{
	return identifier;
}
long  CAN_id::get_eid(  )							// extended id - the full entity
{
	id  = ((long int)block<<24);
	id += ((long int)identifier<<8);
	id += ((long int)channel);
	return id;
}

//=================== MATCHING FUNCTIONS: =======================
bool  CAN_id::match( CAN_id mID )
{
	return (block == mBlock);
}

bool  CAN_id::block_match( byte mBlock )
{
	return (block == mBlock);
}

bool  CAN_id::id_match( word mID)
{
	return (id == mID);
}

bool  CAN_id::instance_match( byte mChannel)
{
	return (channel == mChannel);
}



