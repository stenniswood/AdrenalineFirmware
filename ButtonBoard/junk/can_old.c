#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "leds.h"

struct sCAN msg1;
struct sCAN msg2;
struct sCAN LastReceivedMsg;
BOOL 		NewMessageReceivedFlag = TRUE;


#define RXOK_FLAG 		(CANSTMOB & ( 1 << RXOK))
#define TXOK_FLAG 		(CANSTMOB & ( 1 << TXOK))
#define ANY_ERROR_FLAG 	(CANSTMOB & ANY_ERROR)
volatile int8_t 		data[8];

//***** Reception ISR **********************************
ISR ( CAN_INT_vect )
{
   int8_t savecanpage;
   savecanpage = CANPAGE;         // Save current MOB
   CANPAGE = CANHPMOB & 0xF0;     // Selects MOB with highest priority interrupt
   
   //PORTD |=  0x80;   
   if ( RXOK_FLAG )
   {  
       	can_retrieve_data( &LastReceivedMsg );
       	NewMessageReceivedFlag = TRUE;
       	led_on( 1 );
        //show_byte( data[0], 0);        

		// (Re) Enable Reception 29 bit IDE DLC8 :
      	CANCDMOB = (( 1 << CONMOB1 ) | ( 1 << IDE ) | ( 8 << DLC0));  
      	// Note - the DLC field of the CANCDMO register is updated by the received MOb. 
      	// If the value differs from expected DLC, an error is set
	  	CANSTMOB &= ~(1<<RXOK);      // Reset reason on selected channel
   } 
   else if (TXOK_FLAG)
   {   
   		// We wont get these (as is observed)
   		// 		even though ENTX is enabled.  IE2 for MOB0 needs to be enabled.
   		//CANCDMOB = 0x00;   	  // Disable Transmission
   		CANSTMOB &= ~(1<<TXOK);   // Clear TXOK flag
   }
   CANPAGE = savecanpage;      // Restore original MOB
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
   // SETUP MOB1 for RECEPTION:
   CANPAGE= ( mMobNumber << MOBNB0 );    	// Select MOB n
   CANIE2 = ( 1 << mMobNumber );     		// Enable interrupts on mob1 for reception and transmission
   CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) 	/*| ( 1 << ENTX )*/;   // Enable interrupts on receive

   CANIDM1 = ((mMaskLow & 0x00FF)>>0);    	// 
   CANIDM2 = ((mMaskLow & 0xFF00)>>8);    	//
   CANIDM3 = ((mMaskHigh & 0x00FF)>>0);    	// 
   CANIDM4 = ((mMaskHigh & 0xFF00)>>8);    	// 
   CANCDMOB = ( 1 << CONMOB1) | ( 1 << IDE ) | ( 8 << DLC0);  // Enable Reception 29 bit IDE DLC8
   CANGCON |= ( 1 << ENASTB );         // Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
}

/*****************************************************************************
Name:        	can_init()
Input:			none
Returns:        none
Description:    This function: sets baudrate, enables Mob0 for Reception,
*****************************************************************************/
void can_init(void)
{
   CANGCON = ( 1 << SWRES );	// Software reset
   CANTCON = 0x00;        		// CAN timing prescaler set to 0

   // DISABLE ALL MOBs
   for ( int8_t mob=0; mob<6; mob++ ) { 
      CANPAGE = ( mob << 4 );    // Selects Message Object 0-5
      CANCDMOB = 0x00;           // Disable mob
      CANSTMOB = 0x00;           // Clear mob status register
   }

   // SET BAUD RATE:
   can_set_baud_250K( FALSE );
	
   // SETUP MOB1 for RECEPTION:
   can_setup_receive_mob( 1, 0x0000, 0x0000, 8 );
   
   while (1) { 
     if ((CANGSTA & (1<<ENFG)))  break;  // 1 - CAN controller enable.
   }
   sei();
}


/* MOB0 for transmission */
void can_tx(void) 
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
}

void can_fill_tx_data( byte mMOb_Number, tID mId, byte* mData, byte mDLC )
{
   byte page = 0;
   page |= (mMOb_Number << 4);
   CANPAGE = page;
//   while ((CANEN2 & (_BV(mMOb_Number))) >0) {};  // Wait for MOb 0 to be free

   CANSTMOB = 0x00;     // Clear mob status register
   CANIDT4 = mId.id_array[3];
   CANIDT3 = mId.id_array[2];
   CANIDT2 = mId.id_array[1];
   CANIDT1 = mId.id_array[0];

   for (int i=0; i<mDLC; i++)
   {
      CANMSG = mData[i];
   }
   Can_clear_rtr();   
   CANCDMOB = TRANSMIT | ( 1 << IDE ) | ( mDLC );    // Enable transmission, data length=1 (CAN Standard rev 2.0B(29 bit identifiers))
   
   while ( (CANSTMOB & (1<<TXOK))==0 )	// wait for TXOK flag set
   {
   	// if ANY_ERROR break
   	
   };   

   // todo: have this use interrupts
   //CANCDMOB = 0x00;   // Disable Transmission
   //CANSTMOB = 0x00;   // Clear TXOK flag
}

void can_retrieve_id( struct sCAN* mMsg )
{
	// Retrieve CAN id
	mMsg->id.id_array[0] = CANIDT1;
	mMsg->id.id_array[1] = CANIDT2;
	mMsg->id.id_array[2] = CANIDT3;
	mMsg->id.id_array[3] = CANIDT4;	
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

word can_get_timestamp()
{
  word retval = 0;
  retval = CANSTML;
  retval |= (CANSTMH<<8);
  return retval;
}

/************************************************************
	can_send_msg()
************************************************************/
void can_send_msg( byte mMOb_Number, struct sCAN* mMsg )
{
  can_fill_tx_data( mMOb_Number, mMsg->id, mMsg->data, mMsg->header.DLC );
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
