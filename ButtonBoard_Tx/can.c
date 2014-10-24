#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"

#include "leds.h"

struct sCAN msg1;
struct sCAN msg2;

#define RXOK_FLAG (CANSTMOB & ( 1 << RXOK))
#define TXOK_FLAG (CANSTMOB & ( 1 << TXOK))
#define ANY_ERROR_FLAG (CANSTMOB & ANY_ERROR)
volatile int8_t data[8];

//***** Reception ISR **********************************
ISR ( CAN_INT_vect )
{
   int8_t savecanpage;
   savecanpage = CANPAGE;         // Save current MOB
   CANPAGE = CANHPMOB & 0xF0;      // Selects MOB with highest priority interrupt

   	//static byte Hi = 1;
	//if (Hi) { 	PORTD &=  ~0x80;  Hi = 0; }
	//else    { 
	//PORTD |=  0x80;   
	//Hi = 1; }

   if ( RXOK_FLAG )
   {  
        byte length = ( CANCDMOB & 0x0F );   // DLC, number of bytes to be received
        for ( int8_t i = 0; i < length; i++ )
        {  data[i] = CANMSG;  } 		     // Get data, INDX auto increments CANMSG
        show_byte( data[0], 1);

      CANCDMOB = (( 1 << CONMOB1 ) | ( 1 << IDE ) | ( 8 << DLC0));  // Enable Reception 29 bit IDE DLC8      
      // Note - the DLC field of the CANCDMO register is updated by the received MOb. If the value differs from expected DLC, an error is set
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


/*****************************************************
Input:
	mMobNumber	: Message Object Block to use [0..5]
	mMaskLow	: CANIDM1 & 2
	mMaskHigh	: CANIDM3 & 4
	mDLC		: Data Length Check (0..8)
******************************************************/
void can_receive( byte mMobNumber, short mMaskLow, short mMaskHigh, byte mDLC )
{
   // SETUP MOB1 for RECEPTION:
   CANPAGE= ( mMobNumber << MOBNB0 );    	// Select MOB n
   CANIE2 = ( 1 << mMobNumber );     		// Enable interrupts on mob1 for reception and transmission
   CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) /*| ( 1 << ENTX )*/;   // Enable interrupts on receive

   CANIDM1 = ((mMaskLow & 0x00FF)>>0);    	// 
   CANIDM2 = ((mMaskLow & 0xFF00)>>8);    	//
   CANIDM3 = ((mMaskHigh & 0x00FF)>>0);    	// 
   CANIDM4 = ((mMaskHigh & 0xFF00)>>8);    	// 
   CANCDMOB = ( 1 << CONMOB1) | ( 1 << IDE ) | ( 8 << DLC0);  // Enable Reception 29 bit IDE DLC8
   CANGCON |= ( 1 << ENASTB );         // Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read
}

/* Enables Mob0 for Reception! */
void can_init(void)
{
   CANGCON = ( 1 << SWRES );	// Software reset
   CANTCON = 0x00;        		// CAN timing prescaler set to 0;               
   CANBT1 = 0x0E;         // Set baud rate to 1000kb (assuming 8Mhz IOclk)
   CANBT2 = 0x04;         // ""
   CANBT3 = 0x13;         // ""

   // DISABLE ALL MOBs
   for ( int8_t mob=0; mob<6; mob++ ) { 
      CANPAGE = ( mob << 4 );    // Selects Message Object 0-5
      CANCDMOB = 0x00;           // Disable mob
      CANSTMOB = 0x00;           // Clear mob status register;
   }

   // SETUP MOB1 for RECEPTION:
   CANPAGE= ( 1 << MOBNB0 );     // Select _MOB1_ (yes Mob "1" - b/c 1 shifted into position)
   CANIE2 = ( 1 << IEMOB1 );     // Enable interrupts on mob1 for reception and transmission
   CANGIE = ( 1 << ENIT ) | ( 1 << ENRX ) | ( 1 << ENTX );   // Enable interrupts on receive
   CANIDM1 = 0x00;				// Clear Mask, let all IDs pass
   CANIDM2 = 0x00;				// ""
   CANIDM3 = 0x00;				// ""
   CANIDM4 = 0x00;				// "" 
   CANCDMOB = ( 1<<CONMOB1) | ( 1<<IDE ) | ( 8<<DLC0);  // Enable Reception 29 bit IDE DLC8
   CANGCON |= ( 1<<ENASTB );         // Enable mode. CAN channel enters in enable mode once 11 recessive bits have been read

   while (1) { 
     if ((CANGSTA & (1<<ENFG)))  break;  // 1 - CAN controller enable.
   }
   sei();
}


/*********************************************************************/
/** These adjust the PRS and the SJW bits in the BT2 register       **/
/*********************************************************************/
void can_set_baud_1M()
{
   CANBT1 = BT1_1Mbps;
   CANBT2 = BT2_1Mbps;
   CANBT3 = BT3_1Mbps;
}
void can_set_baud_500K()
{
   CANBT1 = BT1_500Kbps;
   CANBT2 = BT2_500Kbps;
   CANBT3 = BT3_500Kbps;
}
void can_set_baud_250K()
{
   CANBT1 = BT1_250Kbps;
   CANBT2 = BT2_250Kbps;
   CANBT3 = BT3_250Kbps;
}
void can_set_baud_200K()
{
   CANBT1 = BT1_200Kbps;
   CANBT2 = BT2_200Kbps;
   CANBT3 = BT3_200Kbps;
}
void can_set_baud_125K()
{
   CANBT1 = BT1_125Kbps;
   CANBT2 = BT2_125Kbps;
   CANBT3 = BT3_125Kbps;
}
void can_set_baud_100K()
{
   CANBT1 = BT1_100Kbps;
   CANBT2 = BT2_100Kbps;
   CANBT3 = BT3_100Kbps;
}

/* MOB1 for reception    */
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

void can_fill_tx_data( byte mMOb_Number, word mId, byte* mData, byte mDLC )
{
   byte page = 0;
   page |= (mMOb_Number << 4);
   CANPAGE = page;
//   while ((CANEN2 & (_BV(mMOb_Number))) >0) {};  // Wait for MOb 0 to be free

   CANSTMOB = 0x00;     // Clear mob status register
   CANIDT4 = 0x00;      // Set can id to 0
   CANIDT3 = 0x00;      // ""
   CANIDT2 = (mId & 0x07) << 5;      // ""
   CANIDT1 = ((mId>>3) & 0x0F);      // "" 
   
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

/* returns the length of data bytes */
byte can_retrieve_data( byte mMOb_Number, byte* mData )
{
   byte page = 0;
   page |= (mMOb_Number << 4);
  // page |= AINC;        // Auto Increment register addresses (after a write)
   CANPAGE = page;
       // Should Read length out of reg
   for (int i=0; i<8; i++)
   {
      mData[i] = CANMSG;
   }
   return 8;
}

word can_get_timestamp()
{
  word retval = 0;
  retval = CANSTML;
  retval |= (CANSTMH<<8);
  return retval;
}

void can_send_msg( byte mMOb_Number, struct sCAN* mMsg )
{
  can_fill_tx_data( mMOb_Number, mMsg->id, mMsg->data, mMsg->header.Length );
}

void can_get_msg( byte mMOb_Number, struct sCAN* mMsg )
{
  mMsg->header.Length = can_retrieve_data( mMOb_Number, mMsg->data );
  mMsg->time_stamp    = can_get_timestamp();
}

void can_init_test_msg()
{
    msg1.id 	 = 0x0004;
    msg1.data[0] = 0xA0;
    msg1.data[1] = 0x55;    
    msg1.data[2] = 0xA1;
    msg1.data[3] = 0x55;
    msg1.header.Length = 8;
    msg1.header.rtr = 0;
}
