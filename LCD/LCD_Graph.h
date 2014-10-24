

#define MAX_DATA_POINTS 60


void SetGraphPosition(byte mMinColumn, byte mMaxColumn, byte mMinRow, byte mMaxRow);  // LCD placement
void Set_XScaleMinMax(word mMaxValue, word mMinValue);				// Min and Max DATA values.

void ScaleDataPoints();				// Take each data point and scale by the Min & Max 
void Draw_Graph();					// Draw the graph (axes and all)
void AddDataPoint (word mValue);		// Add a data point to the graph
void ResetData();					// Start the index over at the start.

