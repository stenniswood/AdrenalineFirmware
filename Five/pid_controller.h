#ifndef _PID_CONTROLLER_
#define _PID_CONTROLLER_



float control				( float mValue, float mMinValue, float mMaxValue );
float zero_symetric_control	(float mValue, float mMinValue, float mMaxValue  );


/*
	This runs a Proportional, Integral control algorithm. simple untuned.	
	Return	- 	fractional duty [-1.0, 1.0]
	
	call like this:
		float duty = PI_algo( Destination, Encoders[x].Count );				
*/

float PI_algo( float SetPoint, float MeasuredPoint );




#endif
