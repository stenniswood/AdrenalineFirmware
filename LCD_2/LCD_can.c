/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Graphic LCD 128x64 Display of CAN traffic.		 *
 *											  						 *
 *	  CATEGORY:   Mid Level Assorted Routines						 *
 *											   						 *
 *    PURPOSE: 														 *
 *			a) Display generic incoming CAN message				 	 *
 *			b) To browse thru a list of CAN messages				 *
 *			c) To show msgs in place as new data comes in.			 *
 *			d) Define cursor coordinates:  x for lines,			 	 *
 *																	 *
 *	  PIN ASSIGNMENTS:												 *
 *				See "pin_definitions.h"								 *
 *																	 *
 *    AUTHOR: Stephen Tenniswood, Software Engineer 	 			 *
 *			  Sept 19, 2013						 				 	 *
 *********************************************************************/
#include <avr/sfr_defs.h>
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
#include "../core/can.h"
#include "../core/leds.h"
#include "LCD_Text_6x8_chars.h"
#include "LCD_Driver.h"
#include "LCD_Display.h"
#include "can_buff.h"


void show_can_id( struct sCAN* mCAN, BOOL mShowBlock	)
{ 
	byte Block 		= (mCAN->id.group.block & 0x1F);	// 5 bits only valid
	word id    		= (mCAN->id.group.id 		  );
	byte instance	= (mCAN->id.group.instance 	  );
	if (mShowBlock)	{ lcd_draw_byte( Block ); Char_Out('-'); }
	lcd_draw_word( id );
	Char_Out('-');
	lcd_draw_byte( instance );
}

void show_can_data	( struct sCAN* mCAN	)
{
	for (int i=0; i<mCAN->header.DLC; i++)
	{
		lcd_draw_byte( mCAN->data[i] );
		if ( i == 1 )	Char_Out( ' ' );
		if ( i == 3 )	Char_Out( '-' );	// half of full msg
		if ( i == 5 )	Char_Out( ' ' );
	}
}


byte x=0;
byte y=0;
void show_can_msg( struct sCAN* mCAN, byte mRow	)		// Formats an prints the entire message
{
	GotoAddress(mRow, 0);
	Text_Out( "id=" );	show_can_id(mCAN, TRUE);

	Increment_x_address();
	byte x = Get_x_address();		
	GotoAddress(x,19);

	DD_Clear_EOL();	
	show_can_data(mCAN);	

	Increment_x_address();	
}

void show_all_can_msgs(	)
{
	for (int i=0; i<4; i++)
	{
		struct sCAN* Msg = GetMessagePtr( i );
		show_can_msg( Msg, i*2 );
	}
}

