/********************************************************
NAME		:	DC Motor
Description	:	Header file for an extended CAN Id (29 bit)

Product of Beyond Kinetics, Inc.
*********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef CAN_ID_h
#define CAN_ID_h


class CAN_id
{
public:
	CAN_id( word mID, byte mInstance	);
	CAN_id( byte mBlock, word mID, byte mInstance	);
	CAN_id( tID  mID								);	// completed ID
	
	void  set_block( byte mBlock );		// a grouping of ids (5 bits only)
	byte  get_block(  );				// 
	
	void  set_channel( byte mChannel );	// set channel (8 bit) instance id
	byte  get_channel(  );				// 
	
	void  set_id( byte mChannel );		// Message Identifier (16 bit)
	word  get_id(  );					// 
	
	word  get_eid(  );					// extended id - the full entity

	bool  block_match();
	bool  id_match();
	bool  instance_match();
	//CAN_id& operator= (const My_Array & other)

protected:
	byte  block;			// 5 bit
	word  identifier;
	byte  channel;
	
	tID id;			// [0..7] Constructor subtracts 1.
};

#endif


#ifdef __cplusplus
} // extern "C"
#endif

