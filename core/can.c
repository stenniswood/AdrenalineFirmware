/**************************************************************************
Product of Beyond Kinetics, Inc
This code handles the CAN functional block of the atmega16m1.

void set_rx_callback( void (*mCallback)(struct sCAN*) );
void set_tx_callback( void (*mCallback)(struct sCAN*) );

void can_init(void);
void can_setup_receive_mob( byte mMobNumber, short mMaskLow, short mMaskHigh, byte mDLC );
void can_fill_tx_data( byte mMOb_Number, tID mId, byte* mData, byte mDLC );
void can_retrieve_id( struct sCAN* mMsg );

void can_get_msg( struct sCAN* mMsg )
void can_send_msg( byte mMOb_Number, struct sCAN* mMsg )
word can_get_timestamp()
byte can_retrieve_data( struct sCAN* mMsg )
void can_set_id( tID id )

void can_set_baud_100K( BOOL mHighTQ )
void can_set_baud_125K( BOOL mHighTQ )
void can_set_baud_200K( BOOL mHighTQ )
void can_set_baud_250K( BOOL mHighTQ )
void can_set_baud_500K( BOOL mHighTQ )
void can_set_baud_1M  ( BOOL mHighTQ )

**************************************************************************
*****************				MOB LAYOUT					**************
**************************************************************************
6 AVAILABLE MOBS:
MOB0	-  General Transmit			(MyInstance Tagged)
MOB1	-  Module Specific Receives (MyInstance Filtered)
MOB2	-  
MOB3	-  
MOB4	-  System Non-specific Receives 0x001x (Irrelevant MyInstance)
			ID_BOARD_PRESENCE_REQUEST  
MOB5	-  System Non-specific Receives 0x002x (Irrelevant MyInstance)
			ID_INSTANCE_RESERVE_REQUEST 
**************************************************************************
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
**************************************************************************/
#include <stdlib.h>
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "bk_system_defs.h"
#include "can.h"
#include "can_eid.h"
#include "leds.h"
#include "can_board_msg.h"
#include "can_buff.h"
#include "can_instance.h"
#include "can_id_list.h"

struct sCAN 	msg1;
struct sCAN 	msg2;
struct sCAN 	LastReceivedMsg;
BOOL 			NewMessageReceivedFlag = FALSE;
BOOL			TransmittedSuccesfully = FALSE;

volatile int8_t data[8];
void (*rx_call_back)(struct sCAN*) = NULL;	// Call back function for processing specific to the device.
void (*tx_call_back)() = NULL;				// Call back function for processing specific to the device.

//#define USE_TX_INTERRUPTS 1

/*****************************************************************************
Name		: set_rx_callback()
INPUT 		: mCallback - a function ptr to receive incoming CAN msgs
OUTPUT		: none
RETURN		: none
DESCRIPTION:  CAN ISR will call the function sent here on a Receive interrupt
			which is not handled by the can_board_msg.c system message handler
******************************************************************************/
void set_rx_callback( void (*mCallback)(struct sCAN*) )
{
	rx_call_back = mCallback;
}
/*****************************************************************************
Name		: set_tx_callback()
INPUT 		: mCallback - a function ptr to receive incoming CAN msgs.
OUTPUT		: none
RETURN		: none
DESCRIPTION:  CAN ISR will call the function sent here on a Transmit interrupt
			which is not handled by the can_board_msg.c system message handler
*****************************************************************************/
void set_tx_callback( void (*mCallback)() )
{
	tx_call_back = mCallback;
}

// FOR DEBUG!:
extern void protected_led_on(byte mindex);
extern void protected_led_off(byte mindex);

//***** Reception ISR **********************************
ISR ( CAN_INT_vect )
{
   int8_t savecanpage;
   savecanpage = CANPAGE;         // Save current MOB
   int8_t mob = (CANHPMOB & 0xF0) >> 4;
   if (mob == 0x0F)  { CANGIT |= 0; return; }	   
   CANPAGE = CANHPMOB & 0xF0;     // Selects MOB with highest priority interrupt

   if ( ANY_ERROR_FLAG ) {  
		 byte st = CANSTMOB;
		 if (st & BERR)	{ CANSTMOB &= ~(1<<BERR);  /* Bit Error (transmission only) */	};
		 if (st & SERR)	{ CANSTMOB &= ~(1<<SERR);  /* Stuff Error */		 			};
		 if (st & CERR)	{ CANSTMOB &= ~(1<<CERR);  /* CRC Error - pos bad CAN connection(s), bad transmitter, faulty hardware receiver. */ };
		 if (st & FERR)	{ CANSTMOB &= ~(1<<FERR);  /* Form Error */		 				};
		 if (st & AERR)	{ CANSTMOB &= ~(1<<AERR);  /* Acknowledge Error */		 		};

		//protected_led_on( 3 );
		if ((mob==5) || (mob==1)|| (mob==3))		// receives
		{
			CANCDMOB = (( 1 << CONMOB1 ) | ( 1 << IDE ) | ( 8 << DLC0));
			CANSTMOB &= ~(1<<RXOK);      // Reset reason on selected channel
		}
   }
   else if ( RXOK_FLAG ) {   		
       	NewMessageReceivedFlag = TRUE;
       	can_retrieve_data( &LastReceivedMsg );
		BOOL handled = can_board_msg_responder( &LastReceivedMsg );
		if (!handled) {
	       	QueueMessage( &LastReceivedMsg );
	       	if (rx_call_back != NULL)
    	   		rx_call_back( &LastReceivedMsg );
		}

		// (Re)Enable Reception 29 bit IDE DLC8:
      	CANCDMOB = (( 1 << CONMOB1 ) | ( 1 << IDE ) | ( 8 << DLC0));
      	// Note - the DLC field of the CANCDMO register is updated by the received MOb. 
      	// If the value differs from expected DLC, an error is set
	  	CANSTMOB &= ~(1<<RXOK);      // Reset reason on selected channel
   }
   else if (TXOK_FLAG)
   {  
       	if (tx_call_back != NULL)
       		tx_call_back( );			// possibly send next message.

		TransmittedSuccesfully = TRUE;
   		// We wont get these (as is observed)
   		// 		even though ENTX is enabled.  IE2 for MOB0 needs to be enabled
   		CANCDMOB = 0x00;	   	  // Disable Transmission
   		CANSTMOB = 0x00; //~(1<<TXOK);   // Clear TXOK flag
   }		
   CANPAGE = savecanpage;      // Restore original MOB
}

/*****************************************************************************
Name:        	can_init()
Input:			none
Returns:        none
Description:    This function: sets baudrate, enables Mob0 for Reception,
MOB LAYOUT:
0	Module Specific Transmission(MyInstance tagged)
1	Module Specific Receive 	(MyInstance filtered)
2	
3	MyInstance Query 			(autoreply mode)
4	MyInstance Requests			(random instance)
5	Broadcast Receive			( 				)
*****************************************************************************/
void can_init( byte mSpeed )
{
   CANGCON = ( 1 << SWRES );	// Software reset
   CANTCON = 0x00;        		// CAN timing prescaler set to 0

   // DISABLE ALL MOBs
   for ( int8_t mob=0; mob<6; mob++ ) { 
      CANPAGE = ( mob << 4 );    // Selects Message Object 0-5
      CANCDMOB = 0x00;           // Disable mob
      CANSTMOB = 0x00;           // Clear mob status register
   }
   CANGIE = ( 1 << ENIT ) | ( 1 << ENRX );   // Enable interrupts on receive

#ifdef USE_TX_INTERRUPTS
   CANGIE |= ( 1<<ENTX );
#endif

   set_baud(mSpeed);
   can_init_mobs();
   while (1) {
     if ((CANGSTA & (1<<ENFG)))  break;  // 1 - CAN controller enable
   }
   //sei();
}

/*****************************************************************************
Name		: can_init_mobs()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
void can_init_mobs()
{
   // SETUP MOB0 for Module Specific Transmit (MyInstance tagged)
   
   // SETUP MOB1 for Module Specific Reception (MyInstance filtered)
   can_setup_receive_mob	 ( 1, 0x0000, 0x0000, 8 );
   can_add_instance_to_filter( 1, MyInstance 		);

   // SETUP MOB2 Unused
   

   // SETUP MOB3 for ID_INSTANCE_CLAIM (no Instance - checks done in software):
   can_setup_receive_mob		  ( 3, 0x0000, 0x0000, 8 );
   can_remove_instance_from_filter( 3 					 );
   can_add_id_to_filter			  ( 3, ID_INSTANCE_CLAIM, ID_INSTANCE_CLAIM );

   // SETUP MOB4 for ID_INSTANCE_QUERY(Auto-reply):
/* can_setup_receive_mob		  ( 4, 0x0000, 0x0000, 8 );
   can_remove_instance_from_filter( 4 					 );
   can_add_id_to_filter			  ( 4, ID_INSTANCE_QUERY, ID_INSTANCE_QUERY ); */

   // SETUP MOB5 for BROADCAST RECEIVE (no Instance):
   can_setup_receive_mob		  ( 5, 0x0000, 0x0000, 8 );
   can_remove_instance_from_filter( 5 					 );
   can_add_id_to_filter			  ( 5, ID_SYSTEM_REQUEST_MASK, ID_SYSTEM_REQUEST_MASK );
}

/*****************************************************************************
Name :  can_add_instance_to_filter()
Input:  MyInstance variable from can_instance.h include.
OPERATES ON Currently Selected MOb (CANPAGE register)

* This requires that the instance matches this module's MyInstance
   before the message is accepted.  After doing this, the software check 
   is not required for that MOb.
*****************************************************************************/
void can_add_instance_to_filter( byte mMOb_Number, word mInstance )
{
	int8_t savecanpage;
	savecanpage = CANPAGE;         		// Save current MOB
	CANPAGE   = ( mMOb_Number << MOBNB0 );

	byte idt3 = (CANIDT3 & 0xF8) | ((mInstance & 0xE0)>>5);
	byte idt4 = ((mInstance<<3)  | (CANIDT4 & 0x07));

	CANIDT3 = idt3;
	CANIDT4 = idt4;	
	CANIDM3 |= 0x07;
	CANIDM4 |= 0xF8;	
	CANPAGE = savecanpage;
}

/*****************************************************************************
Name :  can_remove_instance_from_filter()
Input:  none
OPERATES ON Currently Selected MOb (CANPAGE register)

* This MOb no longer requires the instance match.  ie It will accept any
  and all Instances. Use this for broadcast messages (intended for all receivers)
*****************************************************************************/
void can_remove_instance_from_filter( byte mMOb_Number )
{
	int8_t savecanpage;
	savecanpage = CANPAGE;					// Save current MOB
	CANPAGE  = ( mMOb_Number << MOBNB0 );	
	CANIDM3 &= 0xF8;
	CANIDM4 &= 0x07;
	CANPAGE = savecanpage;
}

/*****************************************************************************
Name		: can_set_id()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
void can_set_id( byte mMOb_Number, tID id )
{
	int8_t savecanpage;
	savecanpage = CANPAGE;         // Save current MOB
	CANPAGE   = ( mMOb_Number << MOBNB0 );
	
	// Set CAN id
	// Block (5 bits) & Upper 3 of ID
	byte idt1 = ((id.group.block & 0x1F)<<3) | ((id.group.id & 0xE000)>>13);
	byte idt2 = ((id.group.id & 0x1FE0)>>5);
	byte idt3 = ((id.group.id & 0x001F)<<3)  | ((id.group.instance & 0xE0)>>5);
	byte idt4 =  (id.group.instance<<3) | 0x00;		// RTR bits
	CANIDT1 = idt1;
	CANIDT2 = idt2;
	CANIDT3 = idt3;
	CANIDT4 = idt4;
	CANPAGE = savecanpage;
}

/*****************************************************************************
Name :  can_add_id_to_filter()  
			Slightly different from can_set_id() - It only adds the 16bit portion
			of the id to the filter, AND it also sets the mask for this!
			Use when setting up the MOB for specific receive Msgs.
			
Input:  mMOb_Number	- MOb to add the filter to
		mID_mask	- Mask of which bits in the ID portion of the tID are important
		mID_tag		- Tag (value) of the ID bits which must be matched.
* This is done independent of the instance filtering.  
*****************************************************************************/
void can_add_id_to_filter( byte mMOb_Number, word mID_mask, word mID_tag )
{
	tID id   = create_CAN_eid( mID_tag,  0 );
	tID mask = create_CAN_eid( mID_mask, 0 );

	int8_t savecanpage;
	savecanpage = CANPAGE;         // Save current MOB
	CANPAGE   = ( mMOb_Number << MOBNB0 );
	
	// Ignore Block & Instance
	byte idt1 = (CANIDT1 & 0xF8) | ((id.group.id & 0xE000)>>13);
	byte idt2 = ((id.group.id & 0x1FE0)>>5);
	byte idt3 = ((id.group.id & 0x001F)<<3) | (CANIDT3 & 0x07);
	CANIDT1 = idt1;
	CANIDT2 = idt2;
	CANIDT3 = idt3;

	// Ignore Block & Instance
	byte idm1 = (CANIDM1 & 0xF8) | ((mask.group.id & 0xE000)>>13);
	byte idm2 = ((mask.group.id & 0x1FE0)>>5);
	byte idm3 = ((mask.group.id & 0x001F)<<3) | (CANIDM3 & 0x07);
	CANIDM1 = idm1;
	CANIDM2 = idm2;
	CANIDM3 = idm3;
	CANPAGE = savecanpage;
}

/*****************************************************************************
Name :  can_remove_instance_from_filter()
Input:  none
OPERATES ON Currently Selected MOb (CANPAGE register)

* This MOb no longer requires the id match.  ie It will accept any
  and all Instances. Use this for broadcast messages (intended for all receivers)
*****************************************************************************/
void can_remove_id_from_filter( byte mMOb_Number )
{
	int8_t savecanpage;
	savecanpage = CANPAGE;         // Save current MOB	
	CANPAGE   = ( mMOb_Number << MOBNB0 );
	byte idm1 = 0xF8;
	byte idm2 = 0;
	byte idm3 = 0x07;
	CANIDM1 &= idm1;
	CANIDM2 &= idm2;
	CANIDM3 &= idm3;
	CANPAGE = savecanpage;
}

/*****************************************************************************
Name:        	can_set_mask()
Input:
	mMaskLow	: CANIDM1 & 2
	mMaskHigh	: CANIDM3 & 4
Returns:        none
Description:    This function sets up a MOB for reception
*****************************************************************************/
void can_set_mask(byte mMOb_Number, short mMaskLow, short mMaskHigh )
{
	int8_t savecanpage;
	savecanpage = CANPAGE;         // Save current MOB
	CANPAGE   = ( mMOb_Number << MOBNB0 );
	
	CANIDM1 = ((mMaskLow & 0x00FF)>>0);    	
	CANIDM2 = ((mMaskLow & 0xFF00)>>8);    	
	CANIDM3 = ((mMaskHigh & 0x00FF)>>0);    	
	CANIDM4 = ((mMaskHigh & 0xFF00)>>8);
	
	CANPAGE = savecanpage;
}
/*****************************************************************************
Name		: can_set_tag()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
void can_set_tag(byte mMOb_Number, short mTagLow, short mTagHigh )
{
	int8_t savecanpage;
	savecanpage = CANPAGE;         // Save current MOB
	CANPAGE   = ( mMOb_Number << MOBNB0 );
	
	CANIDM1 = ((mTagLow & 0x00FF)>>0);    	// 
	CANIDM2 = ((mTagLow & 0xFF00)>>8);    	// 
	CANIDM3 = ((mTagHigh & 0x00FF)>>0);    	// 
	CANIDM4 = ((mTagHigh & 0xFF00)>>8);    	// 	
	CANPAGE = savecanpage;
}

/*****************************************************************************
Name:        	can_setup_receive_mob()
Input:
	mMobNumber	: Message Object Block to use [0..5]
	mMaskLow	: CANIDM1 & 2
	mMaskHigh	: CANIDM3 & 4
	mDLC		: Data Length Check (0..8)
Returns:        none
Description:    This function sets up a MOB for reception
*****************************************************************************/
void can_setup_receive_mob( byte mMobNumber, short mMaskLow, short mMaskHigh, byte mDLC )
{
	int8_t savecanpage;
	savecanpage = CANPAGE;         			// Save current MOB

	CANPAGE= ( mMobNumber << MOBNB0 );    	// Select MOB n
	// SETUP MOBn for RECEPTION : 
	CANIE2 |= ( 1 << mMobNumber );     		// Enable interrupts on mob1 for reception and transmission		
	can_set_mask( mMobNumber, mMaskLow, mMaskHigh );	
	CANCDMOB = ( 1 << CONMOB1) | ( 1 << IDE ) | ( mDLC << DLC0);  // Enable Reception 29 bit IDE DLC8
	CANGCON |= ( 1 << ENASTB );         // Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
	
	CANPAGE = savecanpage;   
}

/***************************************************************
This setup is common to all code.  Not application specific 
There is 1 free MOB, however, for application specific tricks.
****************************************************************/
/*****************************************************************************
*****************				MOB LAYOUT						**************
6 AVAILABLE MOBS:
MOB0	-  General Transmit			(MyInstance Tagged)
MOB1	-  Module Specific Receives (MyInstance Filtered)
MOB2	-  
MOB3	-  
MOB4	-  System Non-specific Receives 0x001x (Irrelevant MyInstance)
			ID_BOARD_PRESENCE_REQUEST  
MOB5	-  System Non-specific Receives 0xFCxx (Irrelevant MyInstance)
			ID_INSTANCE_RESERVE_REQUEST 
******************************************************************************/

void can_disable_mob( byte mob )
{
	  byte restore = CANPAGE;
      CANPAGE  = ( mob << 4 );    // Selects Message Object 0-5
      CANCDMOB = 0x00;           // Disable mob
      CANSTMOB = 0x00;           // Clear mob status register
      CANPAGE  = restore;
}

/*****************************************************************************
Name		: can_retrieve_id()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
void can_retrieve_id( struct sCAN* mMsg )
{
	// Retrieve CAN id
	word idt1 = CANIDT1;
	word idt2 = CANIDT2;
	word idt3 = CANIDT3;
	word idt4 = CANIDT4;
	mMsg->id.group.block = (idt1 >> 3);		// BLOCK = UPPER NIBBLE+1 (5 BITS)

	word tmpW1 = ((idt1 & 0x07)<<(5+8));	// Lowest 3 make the highest 3 of id
	word tmpW2 = (idt2<<5);					// all 8 bits shifted up snug.
	word tmpW3 = (idt3 & 0xF8)>>3;		// 5 more form the least significant bits.
	mMsg->id.group.id = tmpW1 | tmpW2 | tmpW3;

	tmpW1 = ((idt3 & 0x07)<<5);
	mMsg->id.group.instance = ( tmpW1 | (idt4>>3));		// INSTANCE
}

/*****************************************************************************
Name		: can_retrieve_data()
Parameters	: mVertical	Pixel Line number of display	[0..63]
INPUT 		: mMsg - a sCAN structure must have memory already allocated.
OUTPUT		: mMsg - The data fields of this structure will get updated
RETURN		: DLC
DESCRIPTION:  Operates on the currently selected CANPAGE
*****************************************************************************/
byte can_retrieve_data( struct sCAN* mMsg )
{
	// Retrieve CAN id
	can_retrieve_id( mMsg );

	// Read DLC out of reg
	mMsg->header.DLC = ( CANCDMOB & 0x0F );   // DLC, number of bytes to be received
	for (int i=0; i<mMsg->header.DLC; i++)
	{
		mMsg->data[i] = CANMSG;
	}
	return mMsg->header.DLC;
}

/************************************************************
	can_send_msg()
    Stuff the parameters into the CAN registers 
************************************************************/
void can_send_msg( byte mMOb_Number, struct sCAN* mMsg )
{  
	can_send_msg_no_wait( mMOb_Number, mMsg );

#ifdef USE_TX_INTERRUPTS
   TransmittedSuccesfully = FALSE;
   CANIE2 |= ( 1<<mMOb_Number );				// Enable interrupts on mob1 for reception and transmission
   CANCDMOB = TRANSMIT | ( 1 << IDE ) | ( mMsg->header.DLC );    // Enable transmission, data length=1 (CAN Standard rev 2.0B(29 bit identifiers))
   while (TransmittedSuccesfully==FALSE) { };
/* TX INTERRUPTS DO NOT WORK.  GETS STUCK IN ABOVE LOOP.  THOUGH THE TXOK GOES HIGH.
//	while ( (CANSTMOB & (1<<TXOK))==0 )		// wait for TXOK flag set
//	{   	 if ANY_ERROR break    }; */
#else
   //CANCDMOB = TRANSMIT | ( 1 << IDE ) | ( mMsg->header.DLC );    // Enable transmission, data length=1 (CAN Standard rev 2.0B(29 bit identifiers))
   while ( (CANSTMOB & (1<<TXOK))==0 )	// wait for TXOK flag set
   {   	/* if ANY_ERROR break */   };
#endif
}

void can_send_msg_no_wait( byte mMOb_Number, struct sCAN* mMsg )
{  
   byte page = 0;
   page		|= (mMOb_Number << 4);
   cli();
   CANPAGE = page;

   // while ((CANEN2 & (_BV(mMOb_Number))) >0) {};  // Wait for MOb 0 to be free
   CANSTMOB = 0x00;     	// Clear mob status register
   can_set_id( mMOb_Number, mMsg->id );

   for (int i=0; i<mMsg->header.DLC; i++)
   {
      CANMSG = mMsg->data[i];
   }
   Can_clear_rtr();
   CANCDMOB = TRANSMIT | ( 1 << IDE ) | ( mMsg->header.DLC );    // Enable transmission, data length=1 (CAN Standard rev 2.0B(29 bit identifiers))
   sei();
}

/************************************************************
can_get_timestamp()
  Operates on the currently selected CANPAGE MOB.
  Gets timestamp out of the CAN timer registers 
************************************************************/
word can_get_timestamp()
{
  word retval = 0;
  retval = CANSTML;
  retval |= (CANSTMH<<8);
  return retval;
}

/************************************************************
can_get_msg()
  Operates on the currently selected CANPAGE MOB.
  Adds a timestamp in addition to can_retrieve_data();	
************************************************************/
void can_get_msg( struct sCAN* mMsg )
{
  mMsg->header.DLC = can_retrieve_data( mMsg );
  mMsg->time_stamp = can_get_timestamp();
}

/*****************************************************************************
Name:        	set_baud()
Input:			mSpeed - constant id see below..
Returns:        none
Description:    This function: sets baudrate, enables Mob0 for Reception
*****************************************************************************/
void set_baud(byte mSpeed)
{
	switch (mSpeed)
	{
	case CAN_1M_BAUD   :     can_set_baud_1M  ( FALSE );	break;
	case CAN_500K_BAUD :     can_set_baud_500K( FALSE );	break;
	case CAN_250K_BAUD :     can_set_baud_250K( FALSE );	break;
	case CAN_200K_BAUD :     can_set_baud_200K( FALSE );	break;
	case CAN_125K_BAUD :     can_set_baud_125K( FALSE );	break;
	case CAN_100K_BAUD :     can_set_baud_100K( FALSE );	break;
	default			   :	 can_set_baud_250K( FALSE );
	}
}

/*********************************************************************/
/** These adjust the BAUD RATE and SAMPLING settings		        **/
/*********************************************************************/
void can_set_baud_1M( BOOL mHighTQ )
{
	if (mHighTQ) {
	   CANBT1 = BT1_1Mbps;
	   CANBT2 = BT2_1Mbps;
	   CANBT3 = BT3_1Mbps;	
	} else {
	   CANBT1 = BT1_1Mbps_A;
	   CANBT2 = BT2_1Mbps_A;
	   CANBT3 = BT3_1Mbps_A;
	}
}
void can_set_baud_500K( BOOL mHighTQ )
{
	if (mHighTQ) {
	   CANBT1 = BT1_500Kbps;
	   CANBT2 = BT2_500Kbps;
	   CANBT3 = BT3_500Kbps;
	} else {   
	   CANBT1 = BT1_500Kbps_A;
	   CANBT2 = BT2_500Kbps_A;
	   CANBT3 = BT3_500Kbps_A;
	}
}
void can_set_baud_250K( BOOL mHighTQ )
{
	if (mHighTQ) {
	   CANBT1 = BT1_250Kbps;
	   CANBT2 = BT2_250Kbps;
	   CANBT3 = BT3_250Kbps;
	} else {   
	   CANBT1 = BT1_250Kbps_A;
	   CANBT2 = BT2_250Kbps_A;
	   CANBT3 = BT3_250Kbps_A;
	}
}
void can_set_baud_200K( BOOL mHighTQ )
{
	if (mHighTQ) {
	   CANBT1 = BT1_200Kbps;
	   CANBT2 = BT2_200Kbps;
	   CANBT3 = BT3_200Kbps;
	} else {   
	   CANBT1 = BT1_200Kbps_A;
	   CANBT2 = BT2_200Kbps_A;
	   CANBT3 = BT3_200Kbps_A;
	}
}
void can_set_baud_125K( BOOL mHighTQ )
{
	if (mHighTQ) {
	   CANBT1 = BT1_125Kbps;
	   CANBT2 = BT2_125Kbps;
	   CANBT3 = BT3_125Kbps;
	} else {
	   CANBT1 = BT1_125Kbps_A;
	   CANBT2 = BT2_125Kbps_A;
	   CANBT3 = BT3_125Kbps_A;
	}
}
void can_set_baud_100K( BOOL mHighTQ )
{
	if (mHighTQ) {
	   CANBT1 = BT1_100Kbps;
	   CANBT2 = BT2_100Kbps;
	   CANBT3 = BT3_100Kbps;
	} else {
	   CANBT1 = BT1_100Kbps_A;
	   CANBT2 = BT2_100Kbps_A;
	   CANBT3 = BT3_100Kbps_A;
	}   
}



/* This should already be done before calling!
byte page = 0;
page |= (mMOb_Number << 4);
page |= AINC;        // Auto Increment register addresses (after a write)
CANPAGE = page; */

/* MOB0 for transmission */
/*void can_tx(void) 
{
   CANPAGE = 0x00;      		    		// Select MOb0 for transmission   
   while ((CANEN2 & (1<<ENMOB0))>0) {};		// Wait for ENMOb0=0 to be free

   CANSTMOB = 0x00;     // Clear mob status register
   CANIDT4 = 0x00;      // Set can id to 0
   CANIDT3 = 0x00;      // ""
   CANIDT2 = 0x00;      // ""
   CANIDT1 = 0x00;      // ""   
   for ( int8_t i = 0; i < 8; ++i ) {
        CANMSG = 0x55+i;  // set message data for all 8 bytes to 55 (alternating 1s and 0s
   }
	Can_clear_rtr();
   CANCDMOB = ( 1 << CONMOB0 ) | ( 1 << IDE ) | ( 8 << DLC0 );    // Enable transmission, data length=1 (CAN Standard rev 2.0B(29 bit identifiers))
   //while ( (CANSTMOB & (1<<TXOK))==0 );   // wait for TXOK flag set
   // todo: have this use interrupts
   //CANCDMOB = 0x00;   // Disable Transmission
   //CANSTMOB = 0x00;   // Clear TXOK flag
}*/