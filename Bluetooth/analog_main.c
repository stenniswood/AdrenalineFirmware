/*********************************************************************
Product of Beyond Kinetics, Inc

This code reads the keypad array of buttons
and sends a CAN message with the info

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./button_board.hex

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
//#include "iom16m1.h"
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "leds.h"
#include "can.h"
#include "can_msg.h"
#include "buttons.h"

struct sCAN keydata;

//***** chip Initialization **********************************
void chip_init(void)
{
// If using crystal oscillator of 16 Mhz   
// Note - May want to set the CKDIV8 fuse if MCU is only supposed to run
// at 8MHz so as not to exceed its operating conditions at startup.
//   CLKPR = ( 1 << CLKPCE );       // Set Clock Prescaler change enable
//   CLKPR = 0x01;            		// Clock Division Factor of 2
// Note - CLKPR must be written within 4 clock cycles of setting CLKPCE
   DDRB = 0xFF;    // PORT B to be outputs for LED monotoring
   DDRC = 0x00;    // Inputs, not used
   DDRE = 0x00;    // Inputs, not used
   PORTB = 0xFE;   // ALL LEDs OFF
   PORTC = 0x00;   // Inputs, not used
   PORTD = 0x00;   // Inputs, not used
   PORTE = 0x00;   // Inputs, not used
   PRR   = 0x00;   // Individual peripheral clocks enabled

    DDRC |= 0x04;     		// PC 2, TXCAN Output
    DDRC &= ~0x08;    		// PC 3, RXCAN Input
    PORTC &= ~0x08;   		

	configure_leds();
	
	RESET_LED_1
	RESET_LED_2
	RESET_LED_3
	RESET_LED_4
}

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
		a += 0x11;
		msg1.data[0] = a;
		led_on (1);
		can_send_msg( 0, &msg1 	);
		delay  (one_second/2);
		led_off(1);
		delay  (one_second/2);
	} 
}

//***** main *****************************************************
int main(void)
{
	union uKeyRoster scan;
	
    chip_init();    // Chip initialization
	can_init();		/* Enables Mob0 for Reception! */
	can_init_test_msg();
	byte result = CANSTMOB;
	
	//send_test_msgs();
	
    while (1)
    {  	  
		wait_for_press();
		
		// Result is stored in prev_keys & keys and hopefully keys2
		scan = pack_array( keys );
		can_prep_button_roster_msg( &msg1, scan );
        can_send_msg( 0, &msg1 );
		show_byte( scan.array[0], 1 );
		
//		result = CANEN2;		// shows MOb n in use   mob1 is staying "in use" after 1 can_tx()
//		result = CANSTMOB;		// shows TXOK or ERRs   BERR staying on instead of TXOK 
//		result = CANGSTA;		// shows TXOK or ERRs   TXBSY blinking (retires?)
//		result = 0x83;
//		result = scan.array[0];		
//		show_byte( a, 1 );
		// [0] upper nibble is front row
		// [0] lower        is top row
		// [1] upper nibble is 2nd row
		// [1] lower        is 3rd row
		// [2] upper nibble is 4th row
      /*  for (int j=0; j<4; j++)
        {
        	show_result_toggle(result);
        	delay (one_second/2);
        }  */
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
		
	Prog other ButtonBoard and see if it receives.
		Yes/No?
					
*/