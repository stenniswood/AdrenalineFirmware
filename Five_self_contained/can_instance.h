#ifndef _CAN_INSTANCE_H_
#define _CAN_INSTANCE_H_

#define INSTANCE_TX_MOB 3

////////////////////////////////////
#define NOT_CLAIMED	  0x00
#define CLAIM_PENDING 0x20
#define REPORT		  0x02		// can_instance_init() will set to this on boot, when our claim has already been validated.  Inform other boards.
#define DIRTY 		  0x80
#define CLEAR_REQUEST 0x40
#define CLAIMED	  	  0x01
#define NOT_USING_INSTANCES 0x04
////////////////////////////////////

extern  byte MyInstance;
extern  byte Confirmed;
extern  byte  init_complete;
extern  word  rand_delay; 
extern  byte  rand_instance;

extern byte  init_path;

void setup_instance_claim_mob();
void random_number_adc_init ();
word pick_random_number		();
void can_prep_instance_request( sCAN* mMsg, byte mRandomInstance );
void can_prep_instance_query  ( sCAN* mMsg );

void can_process_instance_request( struct sCAN* mMsg );
void instance_adc_init();

void ObtainInstanceNumber();

void can_instance_init();
void can_instance_timeslice();


#endif


