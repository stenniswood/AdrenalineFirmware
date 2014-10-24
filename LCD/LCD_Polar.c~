/*********************************************************************
 *    PURPOSE: Routines for drawing a polar plot on the LCD		 	 *
 *			   For example for Sonar Plotting.				 		 *
 *											  						 *
 *	  CATEGORY:   MID non application specific.  Application may	 *
 *					call with any data.								 *
 *											  						 *
 *    DESCRIPTION:  x and y shall be thought of in the traditional	 *
 *					way - x is a column, and y is a row on the display.	*
 *					Potentially store graph data in auxilary RAM	 *
 *    AUTHOR: Stephen Tenniswood, Software Engineer 	 			 *
 *			  November 14, 2008						 				 *
 *********************************************************************/
#include <math.h>
//#include "skp_bsp.h"
#include "LCD_ICONS.h"
#include "LCD_Display.h"
#include "APP_CoordinateTransform.h"
#include "LCD_Polar.h"



/**************************** VARIABLES *********************************/
static U8  PLOT_xLeft, PLOT_xRight;		// LCD plot placement left & right
static U8  PLOT_yTop,  PLOT_yBottom;	// LCD plot placement top & bottom
static U16 MaxDataRange;
static U8  MaxLCDRange,LCDCenter;
static U8  NumDataPoints	  = 0;
static U8  CurrentDataPointer = 0;		// holds position for next new data

float 		Data_Angle[MAX_DATA_POINTS];		// real world data
U16 	 	Data_Range[MAX_DATA_POINTS];		// real world data

static U8  	 LCD_Xs[MAX_DATA_POINTS];			// LCD X coordinate of data
static U8  	 LCD_Ys[MAX_DATA_POINTS];			// LCD Y coordinate of data
/***********************************************************************/



/*****************************************************************************
Name:          	POLAR_SetGraphPosition()
Parameters:     mMinColumn	- 
				mMaxColumn	- 
				mMinRow		- 
				mMaxRow		- 
Returns:        none
Description:    Sets the placement on the LCD positions for the graph.
*****************************************************************************/
void POLAR_SetGraphPosition(U8 mMinColumn, U8 mMaxColumn, U8 mMinRow, U8 mMaxRow)
{
	PLOT_xLeft = mMinColumn;
	PLOT_xRight = mMaxColumn;
	
	PLOT_yTop = mMinRow;
	PLOT_yBottom = mMaxRow;	

	LCDCenter    = (PLOT_xRight - PLOT_xLeft)>>1;
	MaxLCDRange  = (PLOT_yBottom-PLOT_yTop);
}

/*****************************************************************************
Name:          	POLAR_Set_XScaleMinMax
Parameters:     none
Returns:        none
Description:    
				Don't really want a mMinRange since all data points would be 
				pulled to a single point.  
*****************************************************************************/
void POLAR_Set_XScaleMinMax(U16 mMaxRange)
{	
	MaxDataRange = mMaxRange;
}

/****************************************************************************
Name:          	POLAR_ResetData()
Parameters:     none
Returns:        none
Description:    Draws the data on the LCD pre-defined graph region
				Must call 
*****************************************************************************/
void POLAR_ResetData()
{
	CurrentDataPointer = 0;
}

/*****************************************************************************
Name:          	POLAR_ClearData()
Parameters:     none
Returns:        none
Description:    Erase all data points.
*****************************************************************************/
void POLAR_ClearData()
{
	U8 i;
	// Initialize Data points:
	for (i=0; i < MAX_DATA_POINTS; i++)
	{
		Data_Angle[i] = 0.0;
		Data_Range[i] = 0;
	}
	NumDataPoints = 0;
	CurrentDataPointer = 0;
}

/*****************************************************************************
Name:          	POLAR_AddDataPoint()
Parameters:     mRadians - the polar angular coordinate
				mValue   - the radius 
Returns:        none
Description:    Stores the raw data only.  Call with each new data point sample.
*****************************************************************************/
void POLAR_AddDataPoint(U16 mRange, float mRadians)
{
	// RETRIEVE DATA:
	Data_Angle[CurrentDataPointer] = mRadians;
	Data_Range[CurrentDataPointer] = mRange;

	// UPDATE NumDataPoints:
	if (CurrentDataPointer > NumDataPoints)
		NumDataPoints = CurrentDataPointer;

	// Get ready for next time:
	CurrentDataPointer++;	
	if (CurrentDataPointer >= MAX_DATA_POINTS)		// Auto wrap around!
		CurrentDataPointer = 0;
	if (NumDataPoints >= MAX_DATA_POINTS)			// Auto stop!
		NumDataPoints = MAX_DATA_POINTS-1;
}


/*****************************************************************************
Name:          	POLAR_ScaleDataPoints()
Parameters:     none
Returns:        none
Description:    Scales the original data to the actual y rows for displaying.
Prerequisite:	Call to Set_XScaleMinMax()
*****************************************************************************/
void POLAR_ScaleDataPoints()
{
	U8  i;
	float LCD_Range;
	float data_range;
	
	for (i=0; i < NumDataPoints; i++)
	{
		data_range = (float)Data_Range[i];
		if (data_range > MaxDataRange)
		{
			data_range = MaxDataRange;
		}
		LCD_Range = (data_range / (float)MaxDataRange) * (float)MaxLCDRange;

		LCD_Xs[i] = ( cosf(Data_Angle[i]) * LCD_Range) + LCDCenter;
		LCD_Ys[i] = PLOT_yBottom - (sinf(Data_Angle[i]) * LCD_Range);		
	}
}

/*****************************************************************************
Name:          	POLAR_Draw_Graph()
Parameters:     none
Returns:        none
Description:    Draws a polar plot of the data on the LCD pre-defined graph region.
				Spans 180 degrees.  
Prerequisite:	Set_XScaleMinMax(), ScaleDataPoints(), SetGraphPosition()
*****************************************************************************/
void POLAR_Draw_Graph()
{
	U8 i;
	
	// DRAW AXES: 
	DD_DrawHorizontalLine( PLOT_yBottom, PLOT_xLeft, PLOT_xRight );
	DD_DrawVerticalLine  ( PLOT_yTop, PLOT_yBottom, (PLOT_xRight-PLOT_xLeft)>>1  );
	
	// DRAW EACH DATA POINT:
	for (i=0; i < NumDataPoints; i++)
	{
		DD_SetPixel( LCD_Ys[i], LCD_Xs[i] );
	}
}
