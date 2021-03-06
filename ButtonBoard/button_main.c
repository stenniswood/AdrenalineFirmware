/*********************************************************************
Product of Beyond Kinetics, Inc

This code reads the keypad array of buttons
and sends a CAN message with the info

INCOMING MESSAGES:
	ID_SYSTEM_LED_REQUEST
	ID_SYSTEM_LED_RELEASE

OUTGOING MESSAGES:
	ID_BUTTON_ROSTER

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./button_board.hex
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Ulfuse:w:0xE4:m

TODO:
	A) Board resets when a button is held for too long.
	B) Not getting a message everytime.
	C) Better debouncing!
	D) Not Saving config / retrieving on power up.


DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include "bk_system_defs.h"
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "../core/leds.h"
#include "can.h"
#include "can_msg.h"
#include "buttons.h"
#include "OS_timers.h"
#include "can_id_list.h"
#include "can_eid.h"
#include "can_instance.h"
#include "can_board_msg.h"
#include "configuration.h"
#include "calibrations.h"

#define CAN_TRANSMIT_CHANNEL1 0
#define CAN_TRANSMIT_CHANNEL2 2
struct sCAN keydata;

void adc_init()
{
	DIDR0 = 0;
	DIDR1 = 0;
	ADCSRA = 0;	// disabled!
}

void init()
{
	cli();
	chip_init   ( );	
	init_leds   ( );
	buttons_init( );	
	delay(40000);					// ~ 1 sec
	read_cal();
	can_init( CAN_250K_BAUD );			/* Enables Mob0 for Reception! */

	config_init();
	can_instance_init();	
	set_configure_callback	( config_change    );			
	//set_rx_callback( can_file_message );	// empty
	sei();
	
	OS_InitTask ( );
}

/* This proc proscribes some specific action to be taken when a button is pressed.
    ie what can Msg to send
*/
byte Instance = 0x01;
void button_magic( byte mKey )
{
	switch(mKey)
	{
	// Front Edge, Left to Right : 
	case 0: can_prep_board_presence_request( &msg2, 0 );
			break;
	case 1: can_prep_board_presence_request( &msg2, 1 );
			break;
	case 2: can_prep_board_presence_request( &msg2, 2 );
			break;
	case 3: can_prep_board_presence_request( &msg2, 3 );
			break;

	// Top Row, Left to Right: 
	case 4: can_prep_system_led_msg( &msg2, Instance, SYSTEM_LED_MODE_DEVICE, 0 );
			break;
	case 5: can_prep_system_led_msg( &msg2, Instance, SYSTEM_LED_MODE_PATTERN, 0x05 );
			break;
	case 6: can_prep_system_led_msg( &msg2, Instance, SYSTEM_LED_MODE_STROBE, 0x00 );
			break;
	case 7: can_prep_system_led_msg( &msg2, Instance, SYSTEM_LED_MODE_STROBE, 0x01 );
			break;
	case 8: can_prep_system_led_msg( &msg2, Instance, SYSTEM_LED_MODE_MYINSTANCE, 0 );
			break;

	case 0x0C:  Instance = 0x01;				break;
	case 0x0D:  Instance = 0x02;				break;
	case 0x0E:  Instance = 0x03;				break;
	case 0x0F:  Instance = 0x04;				break;
	
	case 9: Instance = 0;
			led_off(2);
			led_off(4);
			led_off(1);
			RESET_LED_2();
			break;
	default: break;	
	}
	can_send_msg( CAN_TRANSMIT_CHANNEL2, &msg2 );
}

/* Button Board Test Plan:
	A) Initial Board Alive test, call this below.  Watch LEDs go. 
		led_test_pattern();								TEST PASSED 8/31/13
	B) Enable Receive Interrupts and call Show_byte() in the ISR.
		Have only 1 module transmitting.  At least 2 modules receiving.
		1 showed high nibble.  2nd showed low nibble.	TEST PASSED 8/31/13
	C) Add in button scanning code.  Send Message with data.
		Have 5 modules with 4 leds each showing the nibbles.  
		All buttons should be visible.
	D) Production ready - test debounce reliably 
*/

//***** main *****************************************************
byte count = 0;
int main(void)
{
	union uKeyRoster scan;
    init();    

    while (1)
    {
		wait_for_press();
		// Result is stored in prev_keys & keys and hopefully keys2

		SET_LED_1();
		scan = pack_array( keys );
		build_key_list   ( keys );

		if (isConfigured(MODE_SEND_ROSTER)==0)		{
			can_prep_button_roster_msg( &msg1, scan );
			can_send_msg_no_wait( CAN_TRANSMIT_CHANNEL1, &msg1 );
		}
		if (isConfigured(MODE_SEND_PRESSED)==0)		{
			can_prep_button_pressed_msg( &msg2 );
			can_send_msg_no_wait( CAN_TRANSMIT_CHANNEL2, &msg2 );
	    }
		RESET_LED_1();
    }
    return(0);
} 


/* To summarize Button Board CAN :
		After 1 transmit, the can_tx() routine sticks on the "In Use" loop.
		This is because a Bit Error BERR happens.  Therefore no TXOK.		
			ie.  the RX_CAN monitored differed from the TX_CAN sent.
			
		Therefore the transmitter tries again.  Thus the blinking TXBSY.
		
		Observation of the circuit board.
		Transceiver has RX_CAN tied shorted to ground.

	MAYBE CLOCK IS DIFFERENT ON THE BUTTON BOARD!  SO BAUD RATE NOT COMPATIBLE!
	CHECK FUSES.  YES, the button board had the divide by 8 programmed.
	
	Transmit worked!  Showed up on the Tilt boards 0xf0 as all on.
	Why does the Receive Button board not work?  Maybe that was damaged on the transceiver.
		RXCAN was the one tied to +5V.  short power to ground through it's drive transistor. may have zapped it.

TEST PROCEDURE:		
	Prog other ButtonBoard and see if it receives.
		Yes/No?

	Test Transmitter:
	//send_test_msgs();

	 TEST A)  Initial Board Alive test, call this below.  Watch LEDs go. 
	led_test_pattern();	

	 B) Enable Receive Interrupts and call Show_byte() in the ISR.
		ALL CODE IS IN ISR()	
	while (1) {};	
*/
// Use to verify that multiple boards receive the messages simultaneously
// There was a defect observed when this was sending an 2 other boards were receiving.
// Top nibble was shown on 1 tilt board Leds and lower nibble on another.
// One started lagging the other.  Since this message is sent only once, it means
// 		Maybe a sign error somewhere top nibble is taken as signed char.
//		Or the receiver buffer wasn't getting serviced properly.
//#define BUTTON_MAGIC 1
/*#ifdef BUTTON_MAGIC
		if (NumberKeysPressed==1)
			button_magic( keys_down[0] );
#endif */
		// TEST CODE:

/*		byte test_keys[NUM_ROWS];
		set_rows_output();
		set_switches_input();		// switches are pulled high unless button is pushed
		scan_keys(test_keys);
		can_init_test_msg(test_keys);
		can_send_msg( CAN_TRANSMIT_CHANNEL, &msg1 ); */
