#ifndef _CAN_H_
#define _CAN_H_

// Software Reset (OR'd with hardware reset)
#define can_reset()       { CANGCON |= _BV(SWRES);    }
#define can_disable()     { CANGCON &= ~(1<<ENASTB);  }
#define can_enable()      { CANGCON |= (1<<ENASTB);   }
#define can_full_abort()  { CANGCON |=  (1<<ABRQ); CANGCON &= ~(1<<ABRQ); }

//#define Can_set_mob(mob)       { CANPAGE = ((mob) << 4);  }
#define Can_set_mask_mob()     { CANIDM4=0xFF; CANIDM3=0xFF; CANIDM2=0xFF; CANIDM1=0xFF; }
#define Can_clear_mask_mob()   { CANIDM4=0x00; CANIDM3=0x00; CANIDM2=0x00; CANIDM1=0x00; }
#define Can_clear_status_mob() { CANSTMOB=0x00; }
#define Can_clear_mob()        { U8  volatile *__i_; for (__i_=&CANSTMOB; __i_<&CANSTML; __i_++) { *__i_=0x00 ;}}

#define CONMOB_MSK 			0xC0
#define DISABLE_MOB       ( CANCDMOB &= (~CONMOB_MSK) )
#define Can_mob_abort()   ( DISABLE_MOB )


#define Can_set_dlc(dlc)  ( CANCDMOB |= (dlc)        )
#define Can_set_ide()     ( CANCDMOB |= (1<<IDE)     )
#define Can_set_rtr()     ( CANIDT4  |= (1<<RTRTAG)  )
#define Can_set_rplv()    ( CANCDMOB |= (1<<RPLV)    )

#define Can_clear_dlc()   ( CANCDMOB &= ~DLC_MSK     )
#define Can_clear_ide()   ( CANCDMOB &= ~(1<<IDE)    )
#define Can_clear_rtr()   ( CANIDT4  &= ~(1<<RTRTAG) )
#define Can_clear_rplv()  ( CANCDMOB &= ~(1<<RPLV)   )
    // ----------

#define Can_config_tx()        { DISABLE_MOB; CANCDMOB |= (MOB_Tx_ENA  << CONMOB); }
#define Can_config_rx()        { DISABLE_MOB; CANCDMOB |= (MOB_Rx_ENA  << CONMOB); }
#define Can_config_rx_buffer() {              CANCDMOB |= (MOB_Rx_BENA << CONMOB); }
    // ----------
#define Can_get_dlc()      ((CANCDMOB &  DLC_MSK)     >> DLC   )
#define Can_get_ide()      ((CANCDMOB &  (1<<IDE))    >> IDE   )
#define Can_get_rtr()      ((CANIDT4  &  (1<<RTRTAG)) >> RTRTAG)
    // ----------
#define Can_set_rtrmsk()   ( CANIDM4 |= (1<<RTRMSK) )
#define Can_set_idemsk()   ( CANIDM4 |= (1<<IDEMSK) )
#define Can_clear_rtrmsk() ( CANIDM4 &= ~(1<<RTRMSK) )
#define Can_clear_idemsk() ( CANIDM4 &= ~(1<<IDEMSK) )

/******************** ATMEGA 16M1 DEVICE ******************************************
CAN DRIVER
**********************************************************************************/

#include "can_eid.h"

// CAN MESSAGE STRUCTURE DEFINITION:
struct sCAN
{
  tID 	id;					// CAN uses 29 bits
  struct {
  	byte  rtr    : 1;		// Remote Transmission Request (0=DataFrame; 1=RemoteFrame)
							// a node can request a msg from another node.
	byte DLC : 4;			// 4 bits => [0..15]
  } header;
  byte data[8];				// 
  word time_stamp;          // 
};


#define ANY_ERROR ((1<<BERR) | (1<<SERR) | (1<<CERR) | (1<<FERR) | (1<<AERR))
#define TRANSMIT (1 << CONMOB0)
#define RECEIVE  (1 << CONMOB1)

#define 		RXOK_FLAG 		(CANSTMOB & ( 1 << RXOK))
#define 		TXOK_FLAG 		(CANSTMOB & ( 1 << TXOK))
#define 		ANY_ERROR_FLAG 	(CANSTMOB & ANY_ERROR)

extern struct sCAN 	msg1;
extern struct sCAN 	msg2;
extern struct sCAN  LastReceivedMsg;
extern BOOL 		NewMessageReceivedFlag;
extern BOOL			TransmittedSuccesfully;

// Call back function for processing specific to the device.
extern void (*rx_call_back)(struct sCAN*);		
extern void (*tx_call_back)();		
void set_rx_callback( void (*rx_call_back)(struct sCAN*) );
void set_tx_callback( void (*rx_call_back)() );
// is called inside the RX ISR()

#define CAN_1M_BAUD 	1
#define CAN_500K_BAUD	2
#define CAN_250K_BAUD	3
#define CAN_200K_BAUD	4
#define CAN_125K_BAUD	5
#define CAN_100K_BAUD	6

void can_set_baud_1M	( BOOL mHighTQ );
void can_set_baud_500K	( BOOL mHighTQ );
void can_set_baud_250K	( BOOL mHighTQ );
void can_set_baud_200K	( BOOL mHighTQ );
void can_set_baud_125K	( BOOL mHighTQ );
void can_set_baud_100K	( BOOL mHighTQ );

void can_disable_mob	 ( byte mob 	);
void can_abort			 (				);
void set_baud			 ( byte mSpeed  );
void can_init_mobs		 (				);
void can_init			 ( byte mSpeed = CAN_250K_BAUD );
void can_setup_receive_mob( byte mMobNumber, short mMaskLow, short mMaskHigh, byte mDLC );
void can_retrieve_id	 ( struct sCAN* mMsg 					);
void can_set_id			 ( tID id 								);
byte can_retrieve_data	 ( struct sCAN* mMsg					);
word can_get_timestamp	 (									  	);
void can_send_msg		 ( byte mMOb_Number, struct sCAN* mMsg 	);
void can_send_msg_no_wait( byte mMOb_Number, struct sCAN* mMsg  );
//void can_send_msg_one_try( byte mMOb_Number, struct sCAN* mMsg  );
void can_get_msg		 ( struct sCAN* mMsg 					);

void can_set_mask(byte mMOb_Number, short mMaskLow, short mMaskHigh );
void can_set_tag(byte mMOb_Number, short mTagLow, short mTagHigh );
void can_set_id( byte mMOb_Number, tID id );

void can_add_id_to_filter( byte mMOb_Number, word mID_mask, word mID_tag );
void can_remove_id_from_filter(byte mMOb_Number );

void can_add_instance_to_filter		( byte mMOb_Number, word mInstance );
void can_remove_instance_from_filter( byte mMOb_Number 			);


/******************************************************************
 ********* BAUD RATE SETTINGS ***************16Mhz System Clock****
 ******************************************************************
Example:
	CANBT1 = 0x0E;  // Baud Rate Prescaler (BRP)   x0E=>/14  => 15/16Mhz = 937.5khz  
	CANBT2 = 0x04;  // No resynchronization;   Propagation Time=(2+1)* Tscl (above line) 
	CANBT3 = 0x13;	// Phase 2 Edge error Compensation=(1+1);  Phase 1 = (1+1);  
					// Sample Points (SMP) = 1 use majority detect (of 3 samples)  
 ******************************************************************/
//=========== HIGH FREQUENCY SAMPLING (low TQ) ======================
// ****** The smallest TQ was chosen in each configuration.  ********
#define BT1_1Mbps 0x00
#define BT2_1Mbps 0x0C
#define BT3_1Mbps 0x36

#define BT1_500Kbps 0x02
#define BT2_500Kbps 0x0C
#define BT3_500Kbps 0x37

#define BT1_250Kbps 0x06
#define BT2_250Kbps 0x0C
#define BT3_250Kbps 0x37

#define BT1_200Kbps 0x08
#define BT2_200Kbps 0x0C
#define BT3_200Kbps 0x37

#define BT1_125Kbps 0x0E
#define BT2_125Kbps 0x0C
#define BT3_125Kbps 0x37

#define BT1_100Kbps 0x12
#define BT2_100Kbps 0x0C
#define BT3_100Kbps 0x37

//=========== ALTERNATE LOWER FREQUENCY SAMPLING ======================
#define BT1_1Mbps_A 0x02
#define BT2_1Mbps_A 0x04
#define BT3_1Mbps_A 0x13

#define BT1_500Kbps_A 0x06
#define BT2_500Kbps_A 0x04
#define BT3_500Kbps_A 0x13

/*	Tq = 0.500us 
	8 = 3 + 2 + 2 + 1
*/
#define BT1_250Kbps_A 0x0E
#define BT2_250Kbps_A 0x04
#define BT3_250Kbps_A 0x13

#define BT1_200Kbps_A 0x12
#define BT2_200Kbps_A 0x04
#define BT3_200Kbps_A 0x13

#define BT1_125Kbps_A 0x1E
#define BT2_125Kbps_A 0x04
#define BT3_125Kbps_A 0x13

#define BT1_100Kbps_A 0x26
#define BT2_100Kbps_A 0x04
#define BT3_100Kbps_A 0x13


#endif


