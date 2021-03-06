/*********************************************************************
Quad/PWM Signal Generator board is mostly a Receiver of CAN Messages.
(Primarily an output device)
(as opposed to Analog Module which is primarily an Input device)

This code receives CAN messages and Updates the PWM duty cycles.

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./pwm_board.hex
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Ulfuse:w:0xE4:m
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -U lfuse:r:-:i -U hfuse:r:-:i -U efuse:r:-:i -U lock:r:-:i

DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
Product of Beyond Kinetics, Inc
============================================================================
 Strategy:
	PWM signals are updated from incoming CAN messages.  There are 
	total of 12 CAN msgs assigned for this.
	The Receive interrupt extracts the CAN buffer and updates the
	PWM array.  
	
	The PWM Update frame is sent out on a periodic basis.  ~300 Hz

	Do we send any periodic CAN msg?  	Not for starters.  Ack bit is there.
	Only initialization msgs.
********************************************************************/
#include <avr/sfr_defs.h>
#include "iom16m1.h"
#include "inttypes.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "leds.h"
#include "pwm.h"
#include "can.h"
#include "can_msg.h"
#include "spi_8500.h"
#include "OS_timers.h"


//***** Initialization **********************************
#define TiltInstance 6
void init()
{
	chip_init();
	can_init(CAN_250K_BAUD );
	can_setup_receive_mob( 2, 0x0000, 0x0000, 8 );
	can_add_id_to_filter ( 2, ID_ACCEL_XYZ, ID_ACCEL_XYZ );
	set_rx_callback( can_file_message );

	init_leds();
	//OS_InitTask();
	init_8500();
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
    init();
    word speed = 0x0010;
    char delta = 0x04;
				
    while (1)
    {
//	    strobe_leds( 1 );
//	    led_on(1);
		delay(one_second/100);

		speed += delta;
		if (speed > 0x0FFF) { delta = -delta; }
		if (speed < 0x000F) { delta = -delta; }

		PWM_array[1] = 0x0FFF;		// PWM20
		PWM_array[2] = 0x000F;		// PWM19
		PWM_array[3] = speed; 		//speed;		// PWM18
		PWM_array[4] = speed; 		//speed;		// PWM17

/*		PWM_array[M1] = 0x0FFF;		// PWM20
		PWM_array[M2] = 0x07FF;		// PWM19
		PWM_array[M3] = 0x07FF; 	//speed;		// PWM18
		PWM_array[M4] = 0x0FFF; 	//speed;		// PWM17*/

		PWM_array[M5] = 0x07FF;		// PWM20
		PWM_array[M6] = 0x07FF;		// PWM19
		PWM_array[M7] = 0x07FF; 	//speed;		// PWM18
		PWM_array[M8] = 0x07FF; 	//speed;		// PWM17

//		led_off(1);
//		strobe_leds( 0 );
		delay(one_second/100);
		send_update_frame( TRUE );
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