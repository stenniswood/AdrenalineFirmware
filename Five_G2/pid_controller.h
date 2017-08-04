#ifndef _PID_CONTROLLER_
#define _PID_CONTROLLER_

#include "global.h"


float control				( float mValue, float mMinValue, float mMaxValue );
float zero_symetric_control	( float mValue, float mMinValue, float mMaxValue );


/*
	This runs a Proportional, Integral control algorithm. simple untuned.	
	Return	- 	fractional duty [-1.0, 1.0]
	
	call like this:
		float duty = PI_algo( Destination, Encoders[x].Count );				
*/

void start_pid_control( char letter, uint32_t mDestinationCount );
void print_pid_parameters();


float PI_algo_raw( uint32_t SetPoint, uint32_t MeasuredPoint, float Kp, float Ki, float Kd,
					float& sum_error, float& prev_error );
float PI_algo	 ( char Letter );


int end_point_reached( int mIndex );
int end_point_settled( int mIndex );



#endif
