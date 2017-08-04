#include <math.h>
#include "encoder.h"
#include "motor.h"
#include "differential.h"
#include "protocol.h"



void init_differential()
{
}


// Should be (will be assumed to be)  given in current unit.
void set_circumference	( float Circumference )
{
	FiveMotorConfigData.wheel_circumference = Circumference;
	// Assume that the counts per rev is already given. So counts_per_unit is up-to-date.
			// if cpr is not correct it, setting it will up-date cpu.  so we get it both ways.
	FiveMotorConfigData.counts_per_unit[0] = FiveMotorConfigData.wheel_counts_per_rev/Circumference;	
	FiveMotorConfigData.counts_per_unit[1] = FiveMotorConfigData.wheel_counts_per_rev/Circumference;
}
void set_wheel_separation	( float Distance )
{
	FiveMotorConfigData.wheel_separation = Distance;
}
void set_counts_per_rev	( float Counts )
{
	FiveMotorConfigData.counts_per_unit[0] = Counts;
	FiveMotorConfigData.counts_per_unit[1] = Counts;	
}


/*  Spin robot about the central axis 
	Will end up facing an angle __ degrees from it's original angle 
*/
void spin_robot_degrees( float Degrees )
{
	float circumference 		= M_PI * FiveMotorConfigData.wheel_separation;
	float fraction_of_rotation  = Degrees / 360.0;
	float circumference_requested = circumference * fraction_of_rotation;
	float counts_requested 		= circumference_requested * FiveMotorConfigData.wheel_counts_per_rev;
	
	// Send right wheel backwards by : 
	set_motor_position_v(  Encoders[0].Count - counts_requested );
	set_motor_position_w(  Encoders[1].Count + counts_requested );		
}

void move_forward( float Distance )
{
	float circumference 		= M_PI * FiveMotorConfigData.wheel_circumference;
	float fraction_of_rotation  = Distance / circumference;
	float counts_requested 		= fraction_of_rotation * FiveMotorConfigData.wheel_counts_per_rev;

	set_motor_position_v( Encoders[0].Count + counts_requested );
	set_motor_position_w( Encoders[1].Count + counts_requested );
}

/* Excluding 'counts' unit.  Because must provide which axis.  */
float convert_units( enum eMeasuringUnit mFrom, enum eMeasuringUnit mTo, float mValue )
{
	if ((mFrom==counts) || (mTo==counts))
	{
		return 0.0;
	}
	//float inch_from  = { nan, 1.0, 0.0254, 0.08333333333 };
	//float meter_from = { nan, 39.370079, 1.0, 3.2808399  };
	//float feet_from  = { nan, 12.0, 0.3048, 1.0 		   };

	float retval = 0.0;
	switch (mFrom)
	{
	case inch  : 
		switch (mTo)
		{
		case inch  : return mValue;						break;
		case meter : return mValue * 0.0254;			break;
		case feet  : return mValue / 12.0;				break;
		default : break;
		}
		break;
	case meter :
		switch (mTo) 
		{
		case inch  : return mValue * 39.370079;			break;
		case meter : return mValue * 1.0;				break;
		case feet  : return mValue * 3.2808399;			break;
		default : break;		
		}
		break;
	case feet  : 
		switch (mTo) 
		{
		case inch  : return mValue * 12.0;				break;
		case meter : return mValue * 0.3048;			break;
		case feet  : return mValue * 1.0;				break;
		default : break;		
		}	
	break;
	default: 	break;
	}
	return retval;		
}




/*
	Radius 	- the inside short radius (point to the left wheel)
				must be in the unit selected.
	
*/
void curve_left( float mDegrees, float mRadius, float mSpeed	)
{
	/* We need to:
			a) Figure out distance each wheel is to travel.		DONE
			b) Figure out the speed to run each one at.
	*/
	// First one is just the portion of the circumference of the given radius:
	float fraction_of_circumference = mDegrees / 360.0;
	float inside_distance  = fraction_of_circumference * (M_PI * 2 * mRadius);		// PI * diameter! still in units.
	float outside_distance = fraction_of_circumference * (M_PI * 2 * (mRadius + FiveMotorConfigData.wheel_separation));

	long int inside_counts  = convert_units_to_counts( 'v', inside_distance );
	long int outside_counts = convert_units_to_counts( 'w', outside_distance );	

	// Now setup the movement : 
	set_motor_position_v( Encoders[0].Count + inside_counts   );		// since curve left.
	set_motor_position_w( Encoders[1].Count + outside_counts  );		// since curve left.
	
	// We want both wheels to complete the distance at the same time:
	float time_to_complete = outside_distance / mSpeed;	// take the longest one.
	float inside_speed  = inside_distance / time_to_complete;
	float outside_speed = outside_distance / time_to_complete;	// ie. just mSpeed

	// until we put in speed control, set duty proportional to speed:
	// for now let 24inch/sec = 1.0 fraction
	float K = 24.0;
	set_motor_duty_letter( 'v', inside_speed / K);
	set_motor_duty_letter( 'w', outside_speed / K);
}

/*
	Radius 	- the inside short radius (point to the right wheel)
*/
void curve_right		( float mDegrees, float mRadius, float mSpeed	)
{
	/* We need to:
			a) Figure out distance each wheel is to travel.		DONE
			b) Figure out the speed to run each one at.
	*/
	float    fraction_of_circumference = mDegrees / 360.0;
	float    inside_distance  =  fraction_of_circumference * (M_PI * 2 * mRadius);		// PI * diameter!
	float	 outside_distance =  fraction_of_circumference * (M_PI * 2 * (mRadius + FiveMotorConfigData.wheel_separation));

	float  inside_counts  = convert_units_to_counts( 'v', inside_distance );
	float  outside_counts = convert_units_to_counts( 'w', outside_distance );	

	// Now setup the movement : 
	set_motor_position_v( Encoders[0].Count + inside_counts   );		// since curve left.
	set_motor_position_w( Encoders[1].Count + outside_counts   );		// since curve left.
	
	// We want both wheels to complete the distance at the same time:
	float time_to_complete = outside_distance / mSpeed;	// take the longest one.	
	float inside_speed  = inside_distance / time_to_complete;
	float outside_speed = outside_distance / time_to_complete;

	// until we put in speed control, set duty proportional to speed:
	// for now let 24inch/sec = 1.0 fraction
	float K = 24.0;
	set_motor_duty_letter( 'v', inside_speed / K);
	set_motor_duty_letter( 'w', outside_speed / K);
}


void turn_right			( float mDegrees, float mSpeed	)
{
	float small_radius = convert_units( inch, FiveMotorConfigData.units, 1.0 );
	curve_right( 90.0, small_radius, mSpeed );		// Just a tiny radius. 1 inch
}
void turn_left			( float mDegrees, float mSpeed	)
{
	float small_radius = convert_units( inch, FiveMotorConfigData.units, 1.0 );
	curve_left( 90, small_radius, mSpeed );		// Just a tiny radius. 1 inch
}


