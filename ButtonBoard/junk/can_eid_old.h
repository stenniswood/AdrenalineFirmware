/********************************************************
NAME		:	DC Motor
Description	:	Header file for an extended CAN Id (29 bit)

Product of Beyond Kinetics, Inc.
*********************************************************/
#ifndef CAN_ID_h
#define CAN_ID_h


tID		create_CAN_eid( word mID, byte mInstance				);
tID		create_CAN_eid( byte mBlock, word mID, byte mInstance	);

byte	get_block	  ( tID mID 								);	// Extract the Block number
byte	get_instance  ( tID mID									);	// 
word	get_id		  ( tID mID									);	

bool  match			( tID m1, tID m2 );
bool  block_match	( tID m1, tID m2 );
bool  id_match   	( tID m1, tID m2 );
bool  instance_match( tID m1, tID m2 );


#endif



