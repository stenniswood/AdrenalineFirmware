/*********************************************************************
 *											   						 *
 *    PURPOSE: Routines for drawing a line graph on the LCD		 	 *
 *							 								 		 *
 *											  						 *
 *	  CATEGORY:   BIOS												 *
 *											  						 *
 *    DESCRIPTION:  x and y shall be thought of in the traditional	 *
 *					way - x is a column, and y is a row on the display.														 *
 *																	 *
 *    AUTHOR: Stephen Tenniswood, Software Engineer 	 			 *
 *			  Sept 14, 2008						 				 	 *
 *********************************************************************/
#include "bk_system_defs.h"
#include "pin_definitions.h"

#include "LCD_ICONS.h"
#include "LCD_Display.h"
#include "lcd_graph.h"



word YMinValue, YMaxValue;

byte  xLeft, xRight;
byte  yTop,  yBottom;

byte  NumDataPoints=0;
byte  CurrentDataPointer = 0;			// holds position for next new data.
word DataPoints[MAX_DATA_POINTS];	// real world data
byte  yDataPoint[MAX_DATA_POINTS];	// graph coordinate of data


/*****************************************************************************
Name:          	ResetData()
Parameters:     none
Returns:        none
Description:    Draws the data on the LCD pre-defined graph region.
				Must call 
*****************************************************************************/
void ResetData()
{
	CurrentDataPointer = 0;
	//NumDataPoints = 0;
}

/*****************************************************************************
Name:          	AddDataPoint()
Parameters:     none
Returns:        none
Description:    Draws the data on the LCD pre-defined graph region.
				Must call 
*****************************************************************************/
void AddDataPoint(word mValue)
{
	DataPoints[CurrentDataPointer] = mValue;
	if (CurrentDataPointer > NumDataPoints)
		NumDataPoints = CurrentDataPointer;

	// Get ready for next time:
	CurrentDataPointer++;	
	if (CurrentDataPointer > MAX_DATA_POINTS)		// Auto Wrap Around!
		CurrentDataPointer = MAX_DATA_POINTS;
	if (NumDataPoints > MAX_DATA_POINTS)
		NumDataPoints = MAX_DATA_POINTS;
}

/*****************************************************************************
Name:          	Set_XScaleMinMax
Parameters:     none
Returns:        none
Description:    Draws the data on the LCD pre-defined graph region.
				Must call 
*****************************************************************************/
void Set_XScaleMinMax(word mMaxValue, word mMinValue)
{	
	byte i;
	YMinValue = mMinValue;
	YMaxValue = mMaxValue;
	
	for (i=0; i < MAX_DATA_POINTS; i++)
	{
		yDataPoint[i] = 0;
		DataPoints[i] = 0;
	}
}

/*****************************************************************************
Name:          	SetGraphPosition()
Parameters:     mMinColumn	- 
				mMaxColumn	- 
				mMinRow		- 
				mMaxRow		- 
Returns:        none
Description:    Sets the placement on the LCD positions for the graph.
*****************************************************************************/
void SetGraphPosition(byte mMinColumn, byte mMaxColumn, byte mMinRow, byte mMaxRow)
{
	xLeft = mMinColumn;
	xRight = mMaxColumn;
	
	yTop = mMinRow;
	yBottom = mMaxRow;	
}

/*****************************************************************************
Name:          	ScaleDataPoints()
Parameters:     none
Returns:        none
Description:    Scales the original data to the actual y rows for displaying.
Prerequisite:	Call to Set_XScaleMinMax()
*****************************************************************************/
void ScaleDataPoints()
{
	byte  i;
	word Range = YMaxValue - YMinValue;	
	byte  PixRange = yBottom - yTop;
	unsigned long Numerator = 0;
	
	for (i=0; i < NumDataPoints; i++)
	{
		Numerator = ((unsigned long)(DataPoints[i] - YMinValue)*(unsigned long)PixRange);
		yDataPoint[i] = (  Numerator / (unsigned long)Range);
	}
}

/*****************************************************************************
Name:          	Draw_Graph()
Parameters:     none
Returns:        none
Description:    Draws the data on the LCD pre-defined graph region.
Prerequisite:	Set_XScaleMinMax(), ScaleDataPoints(), SetGraphPosition()
*****************************************************************************/
void Draw_Graph()
{
	byte i;
	DD_DrawHorizontalLine(yBottom, xLeft, xRight );
	DD_DrawVerticalLine  ( yTop, yBottom, xLeft  );
	
	for (i=0; i < NumDataPoints; i++)
	{	
		DD_SetPixel( yBottom - yDataPoint[i], xLeft+i  );
	}
}
