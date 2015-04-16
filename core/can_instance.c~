/*********************************************************************
Product of Beyond Kinetics, Inc
This code handles can messages for board identification.

The complete 29 bit message consists of:	
Bits	Name			Description
-----	----			---------------------------
5 		Block			(for remapping messages)
16 		MESSAGE_ID
8		INSTANCE_ID 	because there may be more than 1 button board put into the network
--------------------------------------------
INCOMING MESSAGES:
	ID_INSTANCE_RESERVE_REQUEST
	ID_INSTANCE_QUERY

OUTGOING MESSAGES:
	ID_INSTANCE_RESERVE_REQUEST
	ID_INSTANCE_QUERY

Note:  We want to protect the instance numbers once they are assigned.
		As it is now, a change to user config_byte_4 can reset the id.
		We'll add a protected config byte which is system use only 
		and can only be changed by 1 specific CAN message (with passcode) 
		ID_INSTANCE_RESTART.
--------------------------------------------
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdlib.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "can.h"
#include "can_msg.h"
#include "can_eid.h"
#include "can_id_list.h"
#include "can_instance.h"
#include "leds.h"
#include "eeprom.h"
#include "configuration.h"

/* Strategy:
		We begin counting from 0 (or down from 255)
		1) Pick a random instance on startup 
		1.5) Send an ID_INSTANCE_RESERVE_REQUEST  Do this as a one-shot only.
		2) If TXOK goes, then save MyInstance into EEPROM
		3) On Error, set a timeout period and wait
		4) If we receive an ID_INSTANCE_RESERVE_REQUEST with less than our attempted id, 
				ignore it.
		5) If we receive an ID_INSTANCE_RESERVE_REQUEST >= MyInstance, then bump to incoming +1
		6) If x amount of time expires without receiving, then resend the MyInstance		
		Repeat

	Once the Instance ID is found for each board, this must be saved into EEPROM 
	so that future sessions can refer to the knee motor with the same id.
	Otherwise we'll have randomized boards and motor movements belonging to knee will go
	to the hip, etc.			
	
	ID Established.  Future Power ups will setup a MOB with auto-reply mode and an ID of 
	ID_INSTANCE_QUERY.  Each module will have an auto-reply MOB setup to be triggered.

	Then send an ID_INSTANCE_QUERY message.  	
	If there is a reply, then we need to re-initiate the assignment.  Or report a DTC trouble code
	
	Each time we add a new module to the network, we'd have to re-assign the hip, knee, and
	ankle motors.  YIKES!
	
	How about sending an instance query to 0x00, and within the reply, the "Confirmed" bit
	will be sent.  Any module which is not confirmed then can scan thru the ID_INSTANCE_QUERY
	until no more replies, and assume this identifier.	
*/

// NEEDS TO BE SAVED INTO EEPROM!
byte	MyInstance 			 = 0x00;
byte 	Confirmed  			 = 0x00;  	// MyInstance was approved on previous boot.	0x80 is dirty bit
byte 	LastReceivedInstance = 0xFF;  
byte 	init_complete 		 = FALSE;	//
word 	rand_delay			 = 0;		// 
byte 	rand_instance		 = 0;		// 
word 	timeout_10ms_mult 	 = 0;

/************************************************
adc_init()  - Used for Random Number generation
Need to work out sharing with "adc.c" module.  
Some platforms use adc.c and others don't
*************************************************/
/* Set up the ADC.  Needed for the current sense and pot	*/
// REFSn bits in ADMUX  (external AREF voltage not connected!)	
#include "adc.h"
#include "pin_definitions.h"

void random_number_adc_init()
{
	// AD Control & Status Register A:
	byte reg = (1<<ADEN) | (1<<ADIF) | 0x03;   // prescaler = 0b011
	ADCSRA = reg;
	//	Digital Input Disable (reduce power consumption)
	//  We are using ADC2, ADC8, ADC9:
	DIDR0  |=  (1<<ADC3D);
	// HighSpeed Mode & AREF Enable internal circuitry.
	ADCSRB = (1<<ADHSM);
}

word pick_random_number()
{
	DDRD &= ~(0x40);		// Make ADC3-PORTD_6-LED4 an input temporarily
	// Sample PD6 (ADC3 - LED3).  Ie. Since this is used during power up, 
	// the LED pin can be turned into an input, the LED should be high impediance 
	// and therefore the lowest bits of ADC should certainly be random.	
	word result = 0;
	word tmp    = 0;

	// SELECT AND START CONVERSION:
	for (int i=0; i<16; i++)
	{
		ADMUX  =  RANDOM_NUMBER_ADC_MUX;
		ADCSRA |= (1<<ADSC);
		while ( (ADCSRA & (1<<ADSC)) > 0) {};
		// PICK UP RESULT: (use lowest 2 bits)
		tmp  = (ADCL & 0x01) << (i % 16);
		result |= (tmp);
		tmp = ADCH;  
	}
	DDRD |= 0x40;		// Set ADC3-PORTD_6-LED4 back to being an output
	return result;
}

/*void tx_instance_callback()
{
	Confirmed = DIRTY;		// save to EEPROM on next timeslice; then => CLAIMED
}*/

void setup_instance_claim_mob()
{
	// To obtain an instance, we need a receive mob:
	// SETUP MOB3 for ID_INSTANCE_CLAIM (no Instance filter - checks done in software)
	can_setup_receive_mob		   ( INSTANCE_TX_MOB, 0x0000, 0x0000, 8 );
	can_remove_instance_from_filter( INSTANCE_TX_MOB 					);
	can_add_id_to_filter		   ( INSTANCE_TX_MOB, ID_INSTANCE_CLAIM, ID_INSTANCE_CLAIM );
}

byte init_path = 0;

void can_instance_init()
{
	random_number_adc_init();
	word tmp = pick_random_number();		// random time delay
	rand_instance = rand_delay = (tmp & 0xFF);	

	// put adc back into mode for POT
	adc_init();

	if (Confirmed==NOT_CLAIMED)
	{	
		// START AT 0; PICK a DELAY TIME
		init_path = 1;
		setup_instance_claim_mob();
		MyInstance= 0;				// start at 0!
	} else {
		// PICK a DELAY TIME
		//MyInstance=0x99;
		init_path  = 2;
		Confirmed = REPORT;
		/* We cannot send a CAN message in this function because it is called from 
		  inside the can_board_msg handler (part of isr() no transmits allowed! b/c they'll
		  generating an infinit loop of interrupts as soon as the transmit is done!)
		  */
	}
	init_path |= 0x80;	
	init_complete = TRUE;
}

byte ts_state = 0;

void can_instance_timeslice()
{
	ts_state = 1;
	if (Confirmed==CLAIMED)	  return;		// nothing to do
	if (init_complete==FALSE) return;		//
	if (rand_delay-- > 0)	  return;		// wait until our appointed time to claim
	rand_delay = 1;							// so that it comes back in here next timeslice
	ts_state = 2;
	
	if (Confirmed==NOT_CLAIMED)
	{
		ts_state = 3;
		Confirmed = CLAIM_PENDING;
		can_prep_instance_request( &msg2, rand_instance );
		can_send_msg_no_wait	 ( 0, &msg2	);	// no wait here!
		timeout_10ms_mult = 300;				// 3 seconds
	}
	if (Confirmed==REPORT)
	{
		ts_state = 7;
		Confirmed = CLAIMED;
		can_prep_instance_request( &msg2, MyInstance );
		can_send_msg_no_wait	 ( 0, &msg2	);	// no wait here!
	}
	if (Confirmed==CLAIM_PENDING)
	{
		ts_state = 4;	
		// Wait for either TXOK or RX ID_INSTANCE_CLAIM (aborts the TX)
		// OR a TIMEOUT
		cli();
		byte restore = CANPAGE;
		CANPAGE 	 = 0x00;

		if (TXOK_FLAG)
			// We finished sending, so we claimed it.
			Confirmed = DIRTY;		// save on next timeslice
			// see "can_board_msg.c"  can_board_msg_responder(),
			//   if an incoming ID_INSTANCE_CLAIM comes in, it will
			//   abort transmission & update Confirmed.
			// It was aborted and MyInstance was bumped.
			// do nothing just wait for next timeslice.
			// Confirmed=NOT_CLAIMED;
		timeout_10ms_mult--;
		if (timeout_10ms_mult == 0)
		{
			//SET_LED_3();
			Confirmed = NOT_USING_INSTANCES;
		}
		CANPAGE = restore;
		sei();		
	}
	if (Confirmed == DIRTY)			// Dirty bit set?
	{
		ts_state = 5;
		Confirmed = CLAIMED;		// don't save next time, just skip the claiming
		cli();  save_configuration();  sei();
		
		// SEND 1 MORE FOR DEBUG PURPOSES:
		//can_prep_instance_request( &msg2, MyInstance );
		//can_send_msg_no_wait( 0, &msg2    );
	}
	else if (Confirmed == CLEAR_REQUEST)	
	{
		ts_state = 6;
		//Confirmed = NOT_CLAIMED;
		//cli(); save_configuration(); sei();
		// SEND 1 MORE FOR DEBUG PURPOSES:
		//can_prep_instance_request( &msg2, MyInstance );
		//can_send_msg_no_wait			 ( 0, &msg2    );
	}
	ts_state |= 0x80;
}

				
/***************************************************************
The ID_RESERVE_INSTANCE_REQUEST msg uses a random number for its 
"instance" since the instance numbers have not been established 
for the boards yet - the can priority mechanism would have no preference
if they each used sequential instance ids.  ie we would have multiple
boards claiming "i want to request instance 0".  With a random number
each board has a priority which can be easily resolved.  Once the 
requests are granted, from then on priority is not a problem if it's sequential
The first data member of the msg contains the id we wish to reserve.

***************************************************************/
//extern byte ResetReason;
void can_prep_instance_request( sCAN* mMsg, byte mRandomInstance )
{
	// instance used to avoid conflicts on the ID_INSTANCE_RESERVE_REQUEST
    mMsg->id 	  	 = create_CAN_eid( ID_INSTANCE_CLAIM, mRandomInstance );
	mMsg->data[0] 	 = MyInstance;		// Requested value
	mMsg->data[1] 	 = Confirmed;
	mMsg->data[2] 	 = rand_instance;
	mMsg->data[3] 	 = init_path;
	mMsg->data[4] 	 = ts_state;
	mMsg->data[5] 	 = sys_config_byte; 
	//mMsg->data[6] 	 = ResetReason;	
    mMsg->header.DLC = 7;
    mMsg->header.rtr = 0;
}

/* We believe we have an instance claimed.
	Make sure it's unique on the network 
	No-reply  */
void can_prep_instance_query( sCAN* mMsg )
{
	// instance used to avoid conflicts on the ID_INSTANCE_RESERVE_REQUEST 
    mMsg->id 	  = create_CAN_eid( ID_INSTANCE_QUERY, MyInstance );
	mMsg->data[0] = MyInstance;		// Requested value
    mMsg->header.DLC    = 1;
    mMsg->header.rtr    = 0;
}

/************************************************************
 We received a request.
 ***********************************************************/
void can_process_instance_request( sCAN* mMsg )
{
	byte instance = mMsg->data[0];
	if (MyInstance > instance)	return;  // ignore since someone else is behind the game.

	MyInstance = instance+1;
}


/*		Add this later after above is working.
		can_prep_instance_query( &msg2);
		can_send_msg( 0, &msg2 );
		if (reply){
			// extract the largest id from the reply 
			// and do an instance claim on it.
			// if it has a reply (2 nodes bad in network), try it's reply largest value.
			// repeat until done.
		}	*/
