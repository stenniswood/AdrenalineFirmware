/* PID - SJT simple P control:

	Error is simplying positional error for now.  We can about speed control later.
	
	P = (Destination - Encoder);
	Kp = (say 5000 - 0) ==> 100% duty;  want full power only when it's that far apart.
										and farther than that distance, we'll cap it
										at 100%	
	so  (5000 - 0) * Kp = 100;	
	so Kp = 1/50 = 0.02;			to give 100% rather than 1.0

	So for a set point of 1000 ===>
		output = control( Kp*(Destination - Encoder), 10, 100 );
		output = control( 20, 10, 100 )

	And for a set point of 10000 ===>
		output = control( Kp*(Destination - Encoder), 10, 100 );
		output = control( 200, 10, 100 )
		
	Then there will be some overshoot:
		so for a set point of 10000 - 10500 ===> -500 error
		output = control( Kp*(-500), 10, 100 );
		output = zero_symetric_control( -10, 10, 100 )
	
	
	Duty = control( Kp * P, 0, 100 );	see function below.
	
	Okay the Set Point (SP) is set when the user issues the "position" command.
		"position v5000.0;"
		
----------------------------------
[-100,-10] [10, +100]

control (mValue, -100, -10 );
control (mValue, +10, +100 );

*/

float Kp = 0.02;		// Proportional constant (error * Kp)
float Ki = 0.00;

float control( float mValue, float mMinValue, float mMaxValue )
{
	float retval = mValue;
	
	if (mValue < mMinValue)
		retval = mMinValue;
	else if (mValue > mMaxValue)
		mValue = mMaxValue;
	return retval;
}

float zero_symetric_control(float mValue, float mMinValue, float mMaxValue )
{
	if (mValue>0.0)
	{		
		control(mValue, +mMinValue, +mMaxValue );		
	} else {
		control(mValue, -mMaxValue, -mMinValue );
	}
}

float PI_algo( float SetPoint, float MeasuredPoint )
{
	float error = (SetPoint - MeasuredPoint);
	float output = (Kp * error) + (Ki * error);	
	return zero_symetric_control( output, 0.05, 1.0 );		// fractional duty cycle.		
}



