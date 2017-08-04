#include <math.h>
#include "encoder.h"
#include "motor.h"
#include "differential.h"


struct stDiffConfig config;


void init_differential()
{
	config.WheelCircumference  =100;
	config.WheelBaseSeparation =100;
	config.EncoderCountsPerRev =500;
	config.RightWheelIndex	   = 0;
	config.LeftWheelIndex	   = 1;
}



void set_circumference	( float Circumference )
{
	config.WheelCircumference = Circumference;
}
void set_wheel_base 	( float Distance )
{
	config.WheelBaseSeparation = Distance;
}
void set_counts_per_rev	( float Counts )
{
	config.EncoderCountsPerRev = Counts;
}


/*  Spin robot about the central axis 
	Will end up facing an angle __ degrees from it's original angle 
*/
void spin_robot_degrees( float Degrees )
{
	float circumference 		= M_PI * config.WheelBaseSeparation;
	float fraction_of_rotation  = Degrees / 360.0;
	float circumference_requested = circumference * fraction_of_rotation;
	float counts_requested 		= circumference_requested * config.EncoderCountsPerRev;
	
	// Send right wheel backwards by : 
	set_motor_position_1(  Encoders[0].Count - counts_requested );
	set_motor_position_2(  Encoders[1].Count + counts_requested );		
}

void move_n_counts( float Counts )
{
	set_motor_position_1(  Encoders[0].Count + Counts );
	set_motor_position_2(  Encoders[1].Count + Counts );	
}

void move_inch( float Distance )
{
	float circumference 		= M_PI * config.WheelCircumference;
	float fraction_of_rotation  = Distance / circumference;
	float counts_requested 		= fraction_of_rotation * config.EncoderCountsPerRev;

	set_motor_position_1( Encoders[0].Count + counts_requested );
	set_motor_position_2( Encoders[1].Count + counts_requested );
}

