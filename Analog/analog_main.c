/*********************************************************************
Product of Beyond Kinetics, Inc

This code reads the keypad array of buttons
and sends a CAN message with the info

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./Analog_board.hex

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
//#include "iom16m1.h"
#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "pin_definitions.h"
#include "leds.h"
#include "buttons.h"
#include "analog.h"
#include "can.h"
#include "can_msg.h"
#include "configuration.h"
#include "calibrations.h"



// Use to verify that multiple boards receive the messages simultaneously
// There was a defect observed when this was sending an 2 other boards were receiving.
// Top nibble was shown on 1 tilt board Leds and lower nibble on another.
// One started lagging the other.  Since this message is sent only once, it means
// 		Maybe a sign error somewhere top nibble is taken as signed char.
//		Or the receiver buffer wasn't getting serviced properly.
byte a = 0x11;
void send_test_msgs()
{
	a = 0x00;
	while (1) 
	{
		a -= 0x11;			// Count down to distinguish clearly from the led_test_pattern()!
		msg1.data[0] = a;
		led_on(1);	led_off(2); led_off(3);
		can_send_msg( 0, &msg1 );
		delay  (one_second/2);
		led_off(1);
		delay  (one_second/2);
	} 
}

//***** main *****************************************************
/*  RECEIVE MSG:
CAN ID :	0x7F7F 7F0			(29 bits - medium priority)
	Mask of Active signals - 8 bytes (4 x 16 bit mask Analog Chip 1,2,3,4)
	Right now read all and send all!
*/

int main(void)
{	
 	cli();
    chip_init();    /* Chip initialization			*/
	init_leds();
	read_cal();
	can_init(CAN_250K_BAUD);		/* Enables Mob0 for Reception!	*/	
	config_init();
	can_instance_init();
	
	an_init();		/* Analog SPI module init 		*/
	can_init_test_msg();

	sei();
	
	/* TEST B)  Button boards configured as receivers;  LEDs showing upper nibble.
		1 Analog board on network will transmit a message.			*/
	send_test_msgs();

    while (1)
    {  	  
		// READ ALL ACTIVE ANALOG SIGNALS:
		an_read_actives();
		
		// TRANSMIT ACTIVE SIGNALS
		can_send_analog_msgs();

		/* Send about every 1 second */
		delay( one_second );
    }
    return(0);
} 


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