#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "serial.h"
#include "motor.h"
#include "encoder.h"
#include "configuration.h"


extern char buff[256];

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

float control( float mValue, float mMinValue, float mMaxValue )
{
	float retval = mValue;
	
	if (mValue < mMinValue)
		retval = mMinValue;
	else if (mValue > mMaxValue)
		retval = mMaxValue;
	return retval;
}

float zero_symetric_control(float mValue, float mMinValue, float mMaxValue )
{
	if (mValue>0.0)
	{		
		return control(mValue, +mMinValue, +mMaxValue );		
	} else {
		return control(mValue, -mMaxValue, -mMinValue );
	}
}

float PI_algo_raw( uint32_t SetPoint, uint32_t MeasuredPoint, 
					float Kp, float Ki, float Kd,
					float& sum_error,
					float& prev_error )
{
	float set_point      = SetPoint;
	float measured_point = MeasuredPoint;
	float error 		 = (set_point - measured_point);
	float deriv_error 	 = (prev_error - error);	
	sum_error 			+= error;
	prev_error 			 = error;
	float output = (Kp * error) + (Ki * sum_error) + (Kd * deriv_error);
	
	// Debug 
/*	char temp[10];
	dtostrf( error, 4, 4, temp );
	sprintf( buff, "PID error=%s;   ", temp );
	send_message( buff ); */
	return zero_symetric_control( output, 0.0, 1.0 );		// fractional duty cycle.		
}

float PI_algo( char Letter )
{
	int index              = get_letter_index( Letter );
	uint32_t MeasuredPoint = get_position    ( Letter );
	uint32_t SetPoint      = mot_states[index].destination;

	return PI_algo_raw( SetPoint, MeasuredPoint, 
						FiveMotorConfigData.Kp[index], 
						FiveMotorConfigData.Ki[index],
						FiveMotorConfigData.Kd[index],
						mot_states[index].sum_error,
						mot_states[index].prev_error );
}


/*void check_position_reached( int mIndex )
{	
	if (mot_states[mIndex].pre_less_than_destination)		// approaching from 0.
	{
		// Stop when greater or equal than the destination (allow coast past)
		if ( Encoders[mIndex].Count >= mot_states[mIndex].destination )
			set_motor_duty_letter( motor_letters[mIndex], 0.0);
	}
	else 	// approaching from infinity
	{
		// Stop when less than or equal than the destination (allow coast past)
		if ( Encoders[mIndex].Count  <= mot_states[mIndex].destination )
			set_motor_duty_letter( motor_letters[mIndex], 0.0);	
	}
}*/

void start_pid_control( char letter, uint32_t mDestinationCount )
{
	set_motor_position_letter( letter, mDestinationCount );
	int index = get_letter_index( letter );
	mot_states[index].PID_control = TRUE;
	mot_states[index].sum_error = 0.0;	
	mot_states[index].prev_error = 0.0;	
	mot_states[index].end_reported = FALSE;
}


int end_point_reached( int mIndex )
{
	uint32_t position = get_position( motor_letters[mIndex] );
	
	if (mot_states[mIndex].pre_less_than_destination)		// approaching from 0.
	{
		// Stop when greater or equal than the destination (allow coast past)
		if ( position >= mot_states[mIndex].destination )
			return TRUE;
	}
	else 	// approaching from infinity
	{
		// Stop when less than or equal than the destination (allow coast past)
		if ( position  <= mot_states[mIndex].destination )
			return TRUE;
	}
	return FALSE;
}

int end_point_settled( int mIndex )
{
	return FALSE;
}

void print_pid_parameters()
{
	// List all Kp and Ki's:
	sprintf( buff, "PID constants: \r\n" );
	send_message(buff);
	for (int m=0; m<5; m++)
	{
		char kp_str[20];
		char ki_str[20];
		char kd_str[20];		
		dtostrf(FiveMotorConfigData.Kp[m], 3, 6, kp_str);
		dtostrf(FiveMotorConfigData.Ki[m], 3, 6, ki_str );
		dtostrf(FiveMotorConfigData.Kd[m], 3, 6, kd_str );
		sprintf(buff, "\t%c  Kp=%s;  Ki=%s;  Kd=%s\r\n", motor_letters[m], kp_str, ki_str, kd_str );
		send_message( buff );
	}
}

