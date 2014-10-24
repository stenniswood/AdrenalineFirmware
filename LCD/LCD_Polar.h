

/**************************** CONSTANTS *********************************/
#define MAX_DATA_POINTS 60


/**************************** VARIABLES *********************************/
extern float 	Data_Angle[MAX_DATA_POINTS];		// real world data
extern U16 	 	Data_Range[MAX_DATA_POINTS];			// real world data


/**************************** FUNCTIONS *********************************/
void POLAR_SetGraphPosition(U8 mMinColumn, U8 mMaxColumn, U8 mMinRow, U8 mMaxRow);  // LCD placement
void POLAR_Set_XScaleMinMax(U16 mMaxValue);							// Min and Max DATA values.

void POLAR_ScaleDataPoints();		// Calculate LCD coordinates for data
void POLAR_Draw_Graph();			// Draw the graph (axes and all)
void POLAR_AddDataPoint(U16 mValue, float mRadians);	// Add a data point to the graph
void POLAR_ResetData();				// Start the index over at the start (retains data and data pt count)
void POLAR_ClearData();				// Erase data and start over.


