/*********************************************************************
 *											   						 *
 *    DESCRIPTION: User Interface Routines 	 						 *
 *											  						 *
 *	  CATEGORY:   BIOS Driver										 *
 *											   						 *
 *    PURPOSE: 														 *
 *			a) Draw scrollbars, 									 *
 *			b) Set positions, max, min								 *
 *																	 *
 *																	 *
 *    AUTHOR: Stephen Tenniswood, Software Engineer 	 			 *
 *			  May 7, 2007						 				 	 *
 *********************************************************************/
//#include "skp_bsp.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"

#include "lcd_icons.h"
#include "lcd_Driver.h"
#include "lcd_Display.h"
#include "lcd_ScrollBar.h"

// Index Variables

// Pixel Variables


/*****************************************************************************
Name		: SB_SetRange()
Parameters	: none
Returns		: none
Description	: Set the Data range.
******************************************************************************/
void SB_SetRange( stHSB_Data* mHSB, word MaxPos, word MinPos)
{
	word temp;
	if (MinPos > MaxPos)
	{
		temp   = MinPos;			// swap
		MinPos = MaxPos;
		MaxPos = temp;
	}
	else if (MinPos == MaxPos)
		MaxPos = MinPos+1;			// To prevent divide by zero.
	
	mHSB->Data_MaxPosition = MaxPos;
	mHSB->Data_MinPosition = MinPos;
	//SB_Increment = ((MaxPos-MinPos));
}

/*****************************************************************************
Name		: SB_SetPosition()
Parameters	: none
Returns		: none
Description	: Set the data position
******************************************************************************/
void SB_SetPosition(stHSB_Data* mHSB, word NewPos)
{
	float temp;
	
	if (NewPos > mHSB->Data_MaxPosition) 
		NewPos = mHSB->Data_MaxPosition;
	else if (NewPos < mHSB->Data_MinPosition)
		NewPos = mHSB->Data_MinPosition;

	mHSB->Data_CurrPosition = NewPos;
	
	// Linearly Interpolate:
	temp = (mHSB->Data_CurrPosition - mHSB->Data_MinPosition);
	temp /= (float)(mHSB->Data_MaxPosition - mHSB->Data_MinPosition);
	temp *= (float)(mHSB->LCD_EndColumn - mHSB->LCD_StartColumn);
	mHSB->LCD_CurrentPosition = (word)temp + mHSB->LCD_StartColumn;
}

/*****************************************************************************
Name		: SB_SetPlacementH
Parameters	: mLine [0..7]
			: mStartColumn 	[0..127]
			: mEndColumn	[0..127]
Returns		: 
Description	: Sets the position on the LCD display. 			  
******************************************************************************/
void SB_SetPlacementH(stHSB_Data* mHSB, byte mLine, byte mStartColumn, byte mEndColumn)
{
	mHSB->LCD_Line 		 = mLine;
	mHSB->LCD_StartColumn = mStartColumn;
	mHSB->LCD_EndColumn 	 = mEndColumn;
}



/*****************************************************************************
Name		: Draw_HScrollBar()
Parameters	: none
Returns		: none
Description	: Draw the horizontal scroll bar at the specified placement and
			  indicator position.
******************************************************************************/
void Draw_HScrollBar(stHSB_Data* mHSB)
{
	byte ls = mHSB->LCD_Line*8;
	byte le = mHSB->LCD_Line*8+7;
	byte erase_data[128];
	byte i;
		
	DD_EraseLine( mHSB->LCD_Line, mHSB->LCD_StartColumn, mHSB->LCD_EndColumn );
	
/*	for (i=0; i<128; i++)
		erase_data[i] = 0;
	TransferBufferFixedLength( erase_data, mHSB->LCD_EndColumn-mHSB->LCD_StartColumn);
*/
	
	// Draw Box:
	DD_DrawBox(ls, le, mHSB->LCD_StartColumn, mHSB->LCD_EndColumn);
	
	// Draw Indicator
	DD_DrawVerticalLine( ls,le, mHSB->LCD_CurrentPosition);

}

/*****************************************************************************
Name		: Draw_ScrollBar()
Parameters	: none
Returns		: none
Description	: Draw the horizontal scroll bar at the specified placement and
			  indicator position.
******************************************************************************/
void Draw_ScrollBar()
{
	/*byte SB_xpos = SB_Position*SB_Increment;
	signed char SB_min, SB_max, i;
	byte Ctrlr = 2;
	WORD Fillbyte = 0;
	byte temp = 1;
	byte x_saddr, x_eaddr;

	
	SB_min = SB_xpos - (SB_Increment>>1);
	SB_max = SB_xpos + (SB_Increment>>1);
	if (SB_min < 0)  SB_min = 0;
	if (SB_max > 63) SB_max = 63;
		
	if (SB_Increment <=8)	
	{
		Fillbyte = 0;		
		for (i=0; i < SB_Increment; i++)
		{
			Fillbyte |=  temp;
			temp = (temp << 1);
		}		   
		// Now figure out how far to rotate the bits:
		Fillbyte = Fillbyte << (SB_min % 8);
	}

	x_saddr = SB_min >> 3;
	x_eaddr = SB_max >> 3;
	
	for (i=0; i < 8; i++)
	{
		GotoAddress( i, MAX_Y-5);
		temp = 0;
		if (i==0)
		{
			temp = 0x01;			
		}
		else if (i==7)
		{
			temp = 0x80;			
		}
		
		if (i== x_saddr)
		{
		   temp = temp | (Fillbyte & 0xFF);
		}
		else if (i==x_eaddr)
		{
		   temp = temp | ((Fillbyte & 0xFF00) >> 8);
		}		
		LCD_write( DATA, 0xFF , Ctrlr );
		LCD_write( DATA, temp , Ctrlr );
		LCD_write( DATA, temp , Ctrlr );
		LCD_write( DATA, 0xFF , Ctrlr );
	}		
	// SB Size is same as SB_Increment	*/
}







