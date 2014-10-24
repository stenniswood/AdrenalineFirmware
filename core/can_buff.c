/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles the buffering of can messages.

There are 2 modes of operation:
A) History buffer (remembering previous n msgs sequentially)

B) ID Based buffer (filing msgs into mailboxes based on ID; remembers the last n distinct IDs received)
					
DATE 	:  10/16/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <avr/sfr_defs.h>
#include <string.h>
#include "inttypes.h"
#include "interrupt.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "../core/can.h"
#include "../core/leds.h"
#include "../core/can_eid.h"
#include "../core/can_id_list.h"
#include "../core/can_instance.h"
#include "can_buff.h"
#include "can_msg.h"
//#include "LCD_Text_6x8_chars.h"
#include "configuration.h"


struct sCAN Received[MAX_CAN_MSG_MEMORY_SIZE];
byte RxHead = 0;
byte RxTail = 0;

byte FindMailBox( tID mID )
{
	for (int i=0; i<MAX_CAN_MSG_MEMORY_SIZE; i++)
		if ( id_match(mID, Received[i].id) )
			return i;
	return 0xFF;
}

void copy_can_msg( struct sCAN* mDest, struct sCAN* mSrc )
{
	memcpy( (void*)mDest, (void*)mSrc, sizeof(struct sCAN) );	
}

void QueueMessage( struct sCAN* mMsg )
{
	if (isConfigured4(NORMAL_HISTORY_OP_MODE))
		addNormalHistory ( mMsg );
	else 
		addMailboxHistory( mMsg );
	//if (isConfigured4(MAILBOX_HISTORY_OP_MODE))		
}

void addNormalHistory( struct sCAN* mMsg )
{
	copy_can_msg( &(Received[RxHead]), mMsg );	
	RxHead++;
	if (RxHead>=MAX_CAN_MSG_MEMORY_SIZE)
		RxHead = 0;
}

void addMailboxHistory( struct sCAN* mMsg )
{
	byte index = FindMailBox( mMsg->id );
	if (index == 0xFF)
	{
		//Text_Out("NewMailBox");
		copy_can_msg( &(Received[RxHead]), mMsg );
		RxHead++;
		if (RxHead>=MAX_CAN_MSG_MEMORY_SIZE)
			RxHead = 0;
	}
	else {
		copy_can_msg( &(Received[index]), mMsg );	
		//	Text_Out("MailBox="); lcd_draw_byte( index );
	}
}

// Return :  TRUE means message was retrieved.  FALSE => none.
struct sCAN* RetrieveMessage( )
{
	if (isConfigured4(NORMAL_HISTORY_OP_MODE))
	{
		if (RxTail == RxHead)
		{
			RxTail = 0;
			RxHead = 0;
			return NULL;
		} else {		
			return GetMessagePtr( RxTail++ );
		}
	}
	return NULL;
}

struct sCAN* GetMessagePtr( byte mIndex )
{
	return &(Received[mIndex]);
}

void CAN_Q_Timeslice()
{
	//SET_LED_1();
	/* The CAN ISR() will be adding the messages to this Que. So
	   here we dispatch them.	*/
	struct sCAN* msg = RetrieveMessage();
	if (msg)
	{
		SET_LED_2();
		if (isConfigured4(NORMAL_HISTORY_OP_MODE))
			can_file_message( msg );
	}
}


