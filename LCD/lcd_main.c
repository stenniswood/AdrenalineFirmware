/*********************************************************************
Product of Beyond Kinetics, Inc

This code reads the keypad array of buttons
and sends a CAN message with the info

AFTER COMPILE, PROGRAM WITH:
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Uflash:w:./LCD.hex
avrdude -p atmega16m1 -b 19200 -c avrisp -P /dev/tty.usbmodemfd131 -Ulfuse:w:0xE4:m

Compiling entire glcd + arduino library requires ~2148*16 = 34,368 bytes
The LCD prototypes only have 16K therefore 2x out of range.

SOLUTIONS:
	Remove arduino library (after all that work to include it!)
		(Preserve this work space and start another)
	Reduce number of fonts.  takes down to 29k
	
 
DATE 	:  8/8/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
//#include "iom16m1.h"
#include <avr/sfr_defs.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "../core/leds.h"
#include "../core/can.h"
#include "can_buff.h"
#include "can_msg.h"
#include "buttons.h"
#include "fonts/allFonts.h"
#include "LCD_Driver.h"
#include "LCD_Text_6x8_chars.h"
#include "LCD_Text_12x16_chars.h"
#include "LCD_Display.h"
#include "LCD_can.h"
#include "../core/pwm.h"
#include "../core/bk_system_defs.h"
#include "backlight.h"
#include "OS_timers.h"
#include "can_instance.h"
#include "can_eid.h"


//***** chip Initialization **********************************
void init()
{
    chip_init();    				// Chip initialization

	// NORMAL CAN INIT + RECEIVING ALL BROADCAST MSGS : 
	can_init(CAN_250K_BAUD);			/* Enables Mob0 for Reception! */
 	can_remove_instance_from_filter( 1 );
 
	// RECEIVING ALL BROADCAST MSGS :
/*	tID Broadcasts = create_CAN_eid( 0xFC00, 0x00 );
	can_remove_id_from_filter( 5 );		// We want to also view all BROADCAST Msgs
	can_set_mask( 5, 0xE007, 0x0000 );	// This covers the 0xFC00 characteristic of BROADCAST mgs.
	can_set_id( 5, Broadcasts ); 	*/

    // INIT MYINSTANCE:
	MyInstance = 0;
	random_number_adc_init();
	can_instance_init();
    set_rx_callback(can_file_message); 
	//set_tx_callback( can_file_message );

	init_leds();
	InitDisplay();
	backlight_init( );
	OS_InitTask();
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
	float duty = 0.0;
	init();
	DD_DrawAllOff();

/*	while(1) {
		strobe_leds(1); 
		strobe_leds(0); delay(one_second/5); 
		set_brightness(duty);
		duty += 0.1;
		if (duty>=1.0) 
				duty = 0.0;
	}; */
	
	//set_brightness(1.0);
	GotoAddress	( 0, 0			);
	Text_Out	( "Adrenaline"	);
	GotoAddress	( 2, 0			);
	Text_Out	( "Scratch" 	);
	GotoAddress	( 4, 0			);
	Text_Out	( "Pad" 		);
	
	//DD_DrawBox ( 10, 54, 10, 115 );
	//set_beep_times( 250, 250, 10 );
	byte row_slot = 0;
	byte FirstMsg = FALSE;
    while (1)
    {
		if (NewMessageReceivedFlag)
		{
			if (FirstMsg)
			{
				FirstMsg = FALSE;
				DD_DrawAllOff();
			}
			NewMessageReceivedFlag = FALSE;
			show_all_can_msgs();
			row_slot    += 2;
			if (row_slot>=6)	row_slot=0;
	       	led_off( 4 );
		}
		//led_on( 4 );				//buzzer( 1 );
		delay(50000);
		//led_off( 4 );				//buzzer( 0 );
    }
    return(0);
} 

/* To summarize LCD Board CAN :	
				
*/