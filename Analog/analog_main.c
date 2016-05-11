/*********************************************************************
Product of Beyond Kinetics, Inc

This code reads the keypad array of buttons
and sends a CAN message with the info

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./Analog_board.hex
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Ulfuse:w:0xE4:m
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uefuse:w:0xFC:m

To compile on MacBook Pro:
Add : /usr/local/CrossPack-AVR-20120217/bin:
to the front of PATH environment variable.  For example:
> PATH=/usr/local/CrossPack-AVR-20120217/bin:$PATH


DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include "iom16m1.h"
#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "pin_definitions.h"
#include "OS_Dispatch.h"
#include "OS_timers.h"
#include "../core/can.h"
#include "can_msg.h"
#include "can_board_msg.h"
#include "leds.h"
#include "analog.h"
#include "can.h"
#include "can_msg.h"
#include "configuration.h"
#include "calibrations.h"


bool okay_to_read_cal = false;

void init()
{
 	cli();
    chip_init  ( );	    			/* Chip initialization			*/
	init_leds  ( );
	delay	   (1000000);			/* ~ 2 sec						*/
	read_configuration   ( );
	can_init   (CAN_250K_BAUD);		/* Enables Mob0 for Reception!	*/	

	config_init		 ( );
	can_instance_init( );
	an_init    		 ( );			/* Analog SPI module init 		*/

    set_rx_callback			( can_file_message );
	set_configure_callback	( config_change    );

	sei		   ( );
	OS_InitTask();	
}

//***** main *****************************************************
/*  RECEIVE MSG:
CAN ID :	0x7F7F 7F0			(29 bits - medium priority)
	Mask of Active signals - 8 bytes (4 x 16 bit mask Analog Chip 1,2,3,4)
	Right now read all and send all!
*/
int main(void)
{	
	init();
    while (1)
    {    
		/* OS_Dispatch reads ALL ACTIVE ANALOG SIGNALS.
			sends CAN report msgs periodically */		
		if (isSysConfigured(CAN_NEW_BOARD))		
			led_on(4);
		else 
			led_off(4);
    }
    return(0);
} 


/*  What does it need?
	A)  Interrupts on the EOC_ pins (portc).
	To implement:
		Set PCINT0, PCINT1, PCINT2 vectors to the routine.
			ie.  ISR ( PCINT1_vect ) { }
		Setup PCICR (bits 0,1,2)
			For Analog Rev 2, we want PCINT12,13,14,15 which corrresponds to 
									  PORTC 4,5,6,7
				This go in ISR( PCINT1_vect ) {}  				
		Enable individual pins in PCMSK1 
		
		Look for flag in PCIFR interrupt flag register.
		Look for specific pin in PINC	
------------------ done:
	C)  Respond to board presence requests.
		CAN_isr() calls can_board_msg_responder()
		CAN Msg is received in , flag set to send response.
		later in can_board_timeslice() which uses can_board_presence_reply() 
		sending over Mob #2

	D)  CAN Transmitter resistor divider?  NOT NEEDED
*/

/* To summarize Analog Board CAN :
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
		
	Prog other ButtonBoard and see if it receives.
		Yes/No?			
*/


// Use to verify that multiple boards receive the messages simultaneously
// There was a defect observed when this was sending and 2 other boards were receiving.
// Top nibble was shown on 1 tilt board Leds and lower nibble on another.
// One started lagging the other.  Since this message is sent only once, it means
// 		Maybe a sign error somewhere top nibble is taken as signed char.
//		Or the receiver buffer wasn't getting serviced properly.


//MyInstance = 0x55;
//config_byte_1 = 0x01;
//config_byte_2 = 0x02;
//config_byte_3 = 0x03;
//save_configuration();	
//can_init_test_msg();
//if (okay_to_read_cal)	read_configuration();

