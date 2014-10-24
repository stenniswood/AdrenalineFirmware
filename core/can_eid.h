/********************************************************
NAME		:	DC Motor
Description	:	Header file for an extended CAN Id (29 bit)

Product of Beyond Kinetics, Inc.
*********************************************************/
#ifndef CAN_ID_h
#define CAN_ID_h

// CAN 2.0B 29 bit Identifier
union idType
{
	long int	full_id;
	byte		id_array[4];
	struct {
		byte	block;
		word	id;
		byte	instance;
	} group;
};
#define tID idType

tID		create_CAN_eid( word mID,    byte mInstance				);
tID		create_CAN_eid( byte mBlock, word mID, byte mInstance	);

byte	get_block	  ( tID mID 								);	// Extract the Block number
byte	get_instance  ( tID mID									);	// 
word	get_id		  ( tID mID									);	

bool  match			( tID m1, tID m2 );
bool  block_match	( tID m1, tID m2 );
bool  id_match   	( tID m1, tID m2 );
bool  instance_match( tID m1, byte m2 );


#endif



