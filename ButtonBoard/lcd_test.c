#include "can_msg.h"
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "leds.h"
#include "can.h"
#include "can_msg.h"
#include "buttons.h"
#include "OS_timers.h"
#include "can_id_list.h"
#include "can_eid.h"


void test_screen()
{
	//can_prep_test_beep( &msg1, 100 );
	can_prep_test_cursorTo( &msg1, 4, 50 );
	can_prep_test_text ( &msg1 );
	can_prep_test_horiz( &msg1, 32 );
	can_prep_test_vert ( &msg1, 115 );
}

void can_prep_test_beep( sCAN* mMsg, word mValue )
{
    mMsg->id 	  = create_CAN_eid( 0x00, ID_LCD_COMMAND, 0x00 );
	mMsg->data[0] = LCD_CMD_BEEP;
	mMsg->data[1] = lo(mValue);
	mMsg->data[2] = lo(mValue);
	mMsg->data[3] = 5;
    mMsg->header.DLC = 4;
    mMsg->header.rtr = 0;
    can_send_msg( 0, mMsg );    
    //delay(one_second*1);
}

void can_prep_test_text( sCAN* mMsg )
{
	char str[8] = "Hi SJT!";
    mMsg->id 	  = create_CAN_eid( 0x00, ID_LCD_DRAW_TEXT, 0x00 );
	for (int i=0; i<8; i++)
		mMsg->data[i] = str[i];
		
    mMsg->header.DLC = 8;
    mMsg->header.rtr = 0;
    can_send_msg( 0, mMsg );
}

void can_prep_test_horiz( sCAN* mMsg, byte mX )
{
    mMsg->id 	  = create_CAN_eid( 0x00, ID_LCD_COMMAND, 0x00 );
	mMsg->data[0] = LCD_CMD_DRAW_HORIZONTAL;
	mMsg->data[1] = mX;
	mMsg->data[2] = 25;
	mMsg->data[3] = 120;
    mMsg->header.DLC = 4;
    mMsg->header.rtr = 0;
    can_send_msg( 0, mMsg );    
}

void can_prep_test_vert( sCAN* mMsg, byte mY )
{
    mMsg->id 	  = create_CAN_eid( 0x00, ID_LCD_COMMAND, 0x00 );
	mMsg->data[0] = LCD_CMD_DRAW_VERTICAL;
	mMsg->data[1] = mY;
	mMsg->data[2] = 10;
	mMsg->data[3] = 60;
    mMsg->header.DLC = 4;
    mMsg->header.rtr = 0;
    can_send_msg( 0, mMsg );    
}

void can_prep_test_cursorTo( sCAN* mMsg, byte mX, byte mY )
{
    mMsg->id 	  = create_CAN_eid( 0x00, ID_LCD_COMMAND, 0x00 );
	mMsg->data[0] = LCD_CMD_CURSOR_TO;
	mMsg->data[1] = mX;
	mMsg->data[2] = mY;
	mMsg->data[3] = 0x00;		// keep font
    mMsg->header.DLC = 4;
    mMsg->header.rtr = 0;
    can_send_msg( 0, mMsg );    
}


