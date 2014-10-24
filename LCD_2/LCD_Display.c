/*********************************************************************
 *											   						 *
 *    DESCRIPTION: Mid-Level Routines for drawing to the display  	 *
 *				   (boxes, graphs, inverse video, etc) 	 		 	 *
 *											  						 *
 *	  CATEGORY:   BIOS												 *
 *											  						 *
 *    PURPOSE: 														 *
 *																	 *
 *    AUTHOR: Stephen Tenniswood, Software Engineer 	 			 *
 *			  May 7, 2007						 				 	 *
 *********************************************************************/
#include <stdio.h>
#include <string.h>

#include "pin_definitions.h"
//#include "skp_bsp.h"
#include "lcd_icons.h"
#include "lcd_driver.h"
#include "LCD_Text_6x8_chars.h"
#include "LCD_Text_12x16_chars.h"


/*****************************************************************************
Name:        	DrawAllOn()
Parameters:  	position	Line number of display
			    *Buff		Buffer containing data to be written to display. 
			    			Last character should be null.
				controller  [1,2, 3(both)]
Returns:        none
Description:    This function turns on all pixels in the display.
*****************************************************************************/
void DD_DrawAllOn()
{
	int i=0;
	int j=0;
	for (i=0; i < 8; i++)
	{
		send_controller_xy_address( BOTH, i, 0 );	
		for (j=0; j<64; j++)
			LCD_write(DATA, 0xFF, BOTH);	// BOTH
	}
}
void DD_DrawAllOff()
{
	int x,y;	
	for (x=0; x < 8; x++)
	{
	  send_controller_xy_address( BOTH, x, 0 );
	  for (y=0; y < 64; y++)	
	    LCD_write(DATA, 00, BOTH);
	}
}
void DD_DrawZigZag()
{					 
//	TransferBuffer( (char*)ZigZag );
}

void DD_EraseLine(byte mLine, byte mStartCol, byte mEndCol)
{
	byte i;
		
	GotoAddress( mLine, mStartCol );
	for (i=mStartCol; i<mEndCol; i++)
	{
	  LCD_write_data( 0x00 );

	}
}

void DD_Clear_EOL()
{
	// Starts from where ever it left off:
	byte y_save = Get_y_address();
	byte y_pixs = (127 - y_save);
	for (int i=0; i<y_pixs; i++)
		LCD_write_data(  0x00 );

	// Restore the Y_address in the controller :
	Set_y_address( y_save );	
	send_controller_y_address();
/*	do 
	{
	  LCD_write( DATA, 0x00 );
//	  y_address = Increment_y_address();
	} 
	while (y_address != 0);
*/	
}

/************************************
 *  X is the length[0..127]
 *  Y is the height[0..8] 
 ************************************/

/*****************************************************************************
Name:        	DrawIcon
Parameters:  	origy		Line number of display	[0..7] 
				origx		column					[0..127]
			    *mIcon		Structure containing the dimensions & data of the 
							icon. 
				
				controller  [1,2, 3(both)]
Description:    This function sends data of the icon to LCD	
*****************************************************************************/
void DD_DrawIcon(byte origx, byte origy, _far struct stIcon* mIcon )
{
	int i=origx;
	_far byte* ptr=0;
	
	for (; i < origx+mIcon->sizex; i++)
	{
		GotoAddress( i, origy );
		ptr = (_far byte*)mIcon->ptr + ((i-origx)*mIcon->sizey);
		TransferBufferFixedLength ((char*)ptr, mIcon->sizey);
	}
}

/*****************************************************************************
Name:        	DD_Inverse_Video
Parameters:  					
Description:    This function inverts the dark & light pixels.  Does one 
				complete line from mStart column to mEndColumn.
*****************************************************************************/
void DD_Inverse_Video(byte mLine, byte mStartColumn, byte mEndColumn )
{
	int i;
	int mController;
	byte  tmp;
	
	for (i=mStartColumn; i < mEndColumn; i++)
	{
		mController = (i<64) ?  1 : 2 ;
		GotoAddress(mLine, i);
		tmp = LCD_read( DATA, mController );
		tmp = LCD_read( DATA, mController );	// Read must be done 2x!
		tmp ^= 0xFF;					// inverse bits
		GotoAddress(mLine, i);
		LCD_write(DATA, tmp, mController);		
	}
}

/*****************************************************************************
Name:        	DrawHorizontalLine
Parameters:  	mVertical	Pixel Line number of display	[0..63] 
				mStart column								[0..127]
				mEnd column									[0..127]
Description:    This function draws a line horizontally on the LCD	
*****************************************************************************/
void DD_DrawHorizontalLine(byte mVertical, byte mStart, byte mEnd)
{
	byte Line 	  = mVertical >> 3;		// divide by 8
	byte FillBit  = mVertical % 8;		// vertical pixel within byte
	byte FillMask = 1 << FillBit;		// shift to proper position.
	byte i;
	byte Controller;
	byte tmp;
	
	if (mVertical > 63)				// 64 is off screen!
		return;	
	
	for (i=mStart; i < mEnd; i++)
	{
		Controller = (i<64) ?  1 : 2;
		GotoAddress(Line, i);
		tmp = LCD_read( DATA, Controller );
		tmp = LCD_read( DATA, Controller );	// Read must be done 2x!
		tmp |= FillMask;						// inverse bits
		GotoAddress(Line, i);
		LCD_write(DATA, tmp, Controller);			
	}
}

/*****************************************************************************
Name:        	DrawVerticalLine
Parameters:  	mStart	Pixel Line number of display	[0..63] 
				mEnd 									[0..63]
				mColumn									[0..127]
Description:    This function draws a line horizontally on the LCD	
*****************************************************************************/
void DD_DrawVerticalLine(byte mStart, byte mEnd, byte mColumn)
{
	byte i,tmp;
	byte Controller = ((mColumn<64) ?  1 : 2);
	byte StartLine 	= mStart >> 3;			// divide by 8
	byte EndLine 	= mEnd   >> 3;			// divide by 8		
	byte FillBit  	= 1 << (mStart % 8);	// vertical pixel within byte
	byte FillMask 	= FillBit;				// shift to proper position.

	// Create Mask:
	while (FillBit < 0x80)
	{
		FillBit  = FillBit << 1;
		FillMask = FillMask | FillBit;			
	} 
	
	// Draw the First line's mask
	GotoAddress(StartLine, mColumn);
	tmp = LCD_read( DATA, Controller );
	tmp = LCD_read( DATA, Controller );	// Read must be done 2x!
	tmp |= FillMask;						// inverse bits
	GotoAddress(StartLine, mColumn);
	LCD_write(DATA, tmp, Controller);

	// Create Bottom Mask:
	FillBit  = 1 << (mEnd % 8);		// vertical pixel within byte
	FillMask = FillBit;				// shift to proper position
	// Create Mask:
	while (FillBit > 0x01)
	{
		FillBit  = FillBit >> 1;
		FillMask = FillMask | FillBit;
	} 

	// Draw the Bottom line's mask
	Controller = (mColumn<64) ?  1 : 2;
	GotoAddress(EndLine, mColumn);
	tmp = LCD_read( DATA, Controller );
	tmp = LCD_read( DATA, Controller );	// Read must be done 2x!
	tmp |= FillMask;						// inverse bits
	GotoAddress(EndLine, mColumn);
	LCD_write(DATA, tmp, Controller);			
	
	// Now any lines in the middle:	
	StartLine = StartLine+1;
	EndLine   = EndLine -1;
	if (EndLine > StartLine)
	{
		for (i=StartLine; i <= EndLine; i++)
		{
			GotoAddress(i, mColumn);
			tmp = LCD_read( DATA, Controller );
			tmp = LCD_read( DATA, Controller );	// Read must be done 2x!
			tmp |= 0xFF;						// inverse bits
			GotoAddress(i, mColumn);
			LCD_write(DATA, tmp, Controller);			
		}
	}
}

/*****************************************************************************
Name:        	SetPixel
Parameters:  	mVertical	Pixel Line number of display	[0..63] 
				mColumn		 column								[0..127]
Description:    This function draws a line horizontally on the LCD	
*****************************************************************************/
void DD_SetPixel(byte mVertical, byte mColumn)
{
	byte Line = mVertical >> 3;		// divide by 8
	byte Fillbit = mVertical % 8;		// vertical pixel within byte
	byte FillMask = 1 << Fillbit;		// shift to proper position.
	byte Controller = 0;
	byte tmp;

	Controller = (mColumn<64) ?  1 : 2;
	GotoAddress(Line, mColumn);
	tmp = LCD_read( DATA, Controller );
	tmp = LCD_read( DATA, Controller );	// Read must be done 2x!
	tmp |= FillMask;						// inverse bits
	GotoAddress(Line, mColumn);
	LCD_write(DATA, tmp, Controller);
}

/*****************************************************************************
Name:        	DrawBox 
Parameters:  	mLinePixStart		[0..63] 
				mLinePixEnd 		[0..63]
				mColumnStart 		[0..127]
				mColumnEnd			[0..127]
Description:    This function draws a line horizontally on the LCD	
*****************************************************************************/
void DD_DrawBox (byte mLinePixStart, byte mLinePixEnd, byte mColumnStart, byte mColumnEnd)
{
	//   
	DD_DrawVerticalLine  ( mLinePixStart, mLinePixEnd, mColumnStart );
	DD_DrawVerticalLine  ( mLinePixStart, mLinePixEnd, mColumnEnd );
	//   Row, 
	DD_DrawHorizontalLine( mLinePixStart, mColumnStart, mColumnEnd );
	DD_DrawHorizontalLine( mLinePixEnd,   mColumnStart, mColumnEnd );
}

/*****************************************************************************
Name:        	Display_Tunnelling_Box
Parameters:  	
Description:    This function draws an ever shrinking rectangle.  Call repetively
				to see all shrinking sizes.
*****************************************************************************/
void DD_Display_Tunnelling_Box()
{
	static byte yl = 0;		// left column
	static byte yr = 128;	// 
	static byte xt = 0;		// top row
	static byte xb = 63;	// 	
	DD_DrawBox( xt,  xb, yl, yr );
	xt++;  xb--;
	yl++;  yr--;
	if (xt > xb)
	{
		yl=0; yr=128;
		xt=0; xb=63;	
	}
}

/*****************************************************************************
Name:        	CalcPixStringLength()
Parameters:  	
Description:    This function calculates length of the string on screen in 
				pixels.
*****************************************************************************/
inline byte DD_CalcPixs_For_String(_far char* mString)
{
	return strlen(mString)*6;
}


/*****************************************************************************
Name:        	Display_YesNo_Box()
Parameters:  	mText is the text to be shown
				mHighlight may have the values:
					0	-  Neither Yes or No will be highlighted.
					1	-  Yes highlight
					2 	-  No highlight					
Description:    This function draws a dialog box.
				to see all shrinking sizes.
*****************************************************************************/
void DD_DrawYesNo_Box(_far char* mText, byte mHighlight)
{
	byte Left,Right;
	//DD_DrawAllOff();
	 
	Left = (128-DD_CalcPixs_For_String(mText)-12)>>2;
	if (Left > 44)
		Left = 44;				// Box not to be smaller than 40 pixs wide.
	Right= 128 - Left;			// Equally centered on display
	
	DD_DrawBox( 20, 52, Left, Right);
	
	// Draw the Text:
	GotoAddress(3, Left+6); 
	Text_Ln( (_far char*)mText );
	Text_Ln( (char*)"" );
	
	// Draw the Yes/No:
	GotoAddress(5, 36); 
	Text_Out ( (char*)" YES ");			// (_far char*)
	GotoAddress(5, 64); 
	Text_Out ( (char*)" NO ");
	
	switch (mHighlight)
	{
		case 0 :	// do nothing!
					break;
					
		case 1 : 	DD_Inverse_Video( 5, 40, 63 );
					break;
					
		case 2 :	DD_Inverse_Video( 5, 68, 85 );
					break;
		default :
					break;	
	}
}


/*****************************************************************************
Name:        	Display_OkCancel_Box()
Parameters:  	mText is the text to be shown
				mHighlight may have the values:
					0	-  Neither Yes or No will be highlighted.
					1	-  Yes highlight
					2 	-  No highlight					
Description:    This function draws a dialog box.
				to see all shrinking sizes.
*****************************************************************************/
void DD_DrawOkCancel_Box(_far char* mText, byte mHighlight)
{
	byte Left,Right;
	DD_DrawAllOff();
	 
	Left = (128-DD_CalcPixs_For_String(mText)-12)>>2;
	if (Left > 44)
		Left = 44;				// Box not to be smaller than 40 pixs wide.
	Right= 128 - Left;			// Equally centered on display
	
	DD_DrawBox( 20, 52, Left, Right);
	
	// Draw the Text:
	GotoAddress(3, Left+6); 
	Text_Ln( (_far char*)mText );
	Text_Ln( (char*)"" );
	
	// Draw the Yes/No:
	GotoAddress(5, 32); 
	Text_Out ( (char*)"Ok");			// (_far char*)
	GotoAddress(5, 70); 
	Text_Out ( (char*)"Cancel");
	
	switch (mHighlight)
	{
		case 0 :	// do nothing!
					break;
					
		case 1 : 	DD_Inverse_Video( 5, 28, 50 );
					break;
					
		case 2 :	DD_Inverse_Video( 5, 68, 106 );
					break;
		default :
					break;	
	}
}
