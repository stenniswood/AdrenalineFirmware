#ifndef _CAN_H_
#define _CAN_H_

// Software Reset (OR'd with hardware reset)
#define can_reset()       { CANGCON |= _BV(SWRES);    }
#define can_disable()     { CANGCON &= ~(1<<ENASTB);  }
#define can_enable()      { CANGCON |= (1<<ENASTB);   }
#define can_full_abort()  { CANGCON |=  (1<<ABRQ); CANGCON &= ~(1<<ABRQ); }

#define Can_set_mob(mob)       { CANPAGE = ((mob) << 4);  }
#define Can_set_mask_mob()     { CANIDM4=0xFF; CANIDM3=0xFF; CANIDM2=0xFF; CANIDM1=0xFF; }
#define Can_clear_mask_mob()   { CANIDM4=0x00; CANIDM3=0x00; CANIDM2=0x00; CANIDM1=0x00; }
#define Can_clear_status_mob() { CANSTMOB=0x00; }
#define Can_clear_mob()        { U8  volatile *__i_; for (__i_=&CANSTMOB; __i_<&CANSTML; __i_++) { *__i_=0x00 ;}}

#define DISABLE_MOB       ( CANCDMOB &= (~CONMOB_MSK) )
#define Can_mob_abort()   ( DISABLE_MOB )
    // ----------
#define Can_set_dlc(dlc)  ( CANCDMOB |= (dlc)        )
#define Can_set_ide()     ( CANCDMOB |= (1<<IDE)     )
#define Can_set_rtr()     ( CANIDT4  |= (1<<RTRTAG)  )
#define Can_set_rplv()    ( CANCDMOB |= (1<<RPLV)    )
    // ----------
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

struct sCAN
{
  word id;			// CAN uses 11 bits
  struct {
  	byte  rtr    : 1;	// Remote Transmission Request (0=DataFrame; 1=RemoteFrame)
				// a node can request a msg from another node.
	byte Length : 4;	// [0..15]
  } header;
  byte data[8];			// 
  word time_stamp;              // 
};

/******************** ATMEGA 16M1 DEVICE ******************************************
CAN DRIVER DEMO
Initializes the CAN connection.
Sends simple message when button is pushed.
Receives messages.
**********************************************************************************/

/******************************************************************
 ********* BAUD RATE SETTINGS ***************16Mhz System Clock****
 ****** The smallest TQ was chosen in each configuration.  ********/
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

#define ANY_ERROR ((1<<BERR) | (1<<SERR) | (1<<CERR) | (1<<FERR) | (1<<AERR))
#define TRANSMIT (1 << CONMOB0)
#define RECEIVE  (1 << CONMOB1)

extern struct sCAN msg1;
extern struct sCAN msg2;

void can_init(void);
void can_tx	 (void);

void can_set_baud_1M();
void can_set_baud_500K();
void can_set_baud_250K();
void can_set_baud_200K();
void can_set_baud_125K();
void can_set_baud_100K();

word can_get_timestamp	(									  );
void can_fill_tx_data	( byte mMOb_Number, word mId, byte* mData, byte mLength );
byte can_retrieve_data	( byte mMOb_Number, byte* mData 	  );
void can_send_msg		( byte mMOb_Number, struct sCAN* mMsg );
void can_get_msg		( byte mMOb_Number, struct sCAN* mMsg );
void can_init_test_msg	(									  );



#endif


