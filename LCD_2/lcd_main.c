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
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "leds.h"
#include "can.h"
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
#include "can_board_msg.h"
#include "backlight.h"
#include "OS_timers.h"
#include "can_instance.h"
#include "can_eid.h"
#include "calibrations.h"
#include "configuration.h"


//***** chip Initialization **********************************
void init()
{
	cli();
    chip_init();    					// Chip initialization

	// NORMAL CAN INIT + RECEIVING ALL BROADCAST MSGS : 
	read_cal();
	can_init(CAN_250K_BAUD);			/* Enables Mob0 for Reception! */
 	can_remove_instance_from_filter( 1 ); 

    // INIT MYINSTANCE:
 	config_init();
 	can_instance_init();

 	can_prep_instance_request( &msg2, rand_instance );
	can_send_msg_no_wait	 ( 0, &msg2	);	
	
    //set_rx_callback( can_file_message); No, this is handled in CAN_Q_Timeslice()
	set_configure_callback	( config_change    );
    sei();
    
    // Time to draw is too slow to handle real-time!
	init_leds  ();
	InitDisplay();
	backlight_init( );
	OS_InitTask();
}

void title_screen()
{
	DD_DrawAllOff();
	byte startx = dCenterLargeText(6, 0, 128 );
	GotoAddress	( 0, startx	);
	dText_Out	( "Beyond"	);
	startx = dCenterLargeText(8, 0, 128 );
	GotoAddress	( 2, startx		);
	dText_Out	( "Kinetics"	);	
	GotoAddress	( 5, 0			);
	dText_Out	( "ScratchPad"  );
}

//***** main *****************************************************
int main(void)
{
	init();
	title_screen();					
	set_contrast( 0.5 );		// CONTRAST		
	strobe_leds(1); 
	set_brightness(0.5);
	

	byte FirstMsg = TRUE;
    while (1)
    {
		if (NewMessageReceivedFlag)
		{
			NewMessageReceivedFlag = FALSE;
	       	led_off( 4 );
			// Normal mode is to handle screen draw commands.
			// When disabled, show CAN messages.
	       	if (isConfigured4(NORMAL_HISTORY_OP_MODE)==FALSE)
	       	{
				if (FirstMsg) 
				{
					//DD_DrawAllOff();
					// reconfigure mobs for viewing all traffic.					
	       			FirstMsg = FALSE;
	       		}
				//show_all_can_msgs();
			}
		}
		delay(2000);
    }
    return(0);
} 

// Use to verify that multiple boards receive the messages simultaneously
// There was a defect observed when this was sending an 2 other boards were receiving.
// Top nibble was shown on 1 tilt board Leds and lower nibble on another.
// One started lagging the other.  Since this message is sent only once, it means
// 		Maybe a sign error somewhere top nibble is taken as signed char.
//		Or the receiver buffer wasn't getting serviced properly.
/*byte a = 0x11;
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
} */
/*	while(1) {
		delay(one_second/2);
//		set_brightness(duty);
		duty += 0.1;
		if (duty>=1.0)
			duty = 0.0;
	}; */

/* To summarize LCD Board CAN :	
				
*/
	// RECEIVING ALL BROADCAST MSGS :
/*	tID Broadcasts = create_CAN_eid( 0xFC00, 0x00 );
	can_remove_id_from_filter( 5 );		// We want to also view all BROADCAST Msgs
	can_set_mask( 5, 0xE007, 0x0000 );	// This covers the 0xFC00 characteristic of BROADCAST mgs.
	can_set_id( 5, Broadcasts ); 	 */
