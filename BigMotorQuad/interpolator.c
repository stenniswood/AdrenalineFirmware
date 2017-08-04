#include "encoder.h"
#include "interpolator.h"

/* 
   Interpolating - Implies 2 end points.  
   		G code only specifies next end point.		
		So we always maintain the internal position (which is what's in the encoders)

Example Overall Move : 
M1,2,3,4 == 100;  100; 	100;  100;
G01  W356.00 X256.00 Y512.25 Z2048.88 

We want all motors to finish at the same time!
D = velocity * time;
So we find a velocity for each motor to reach the D.


D0 = 356.00-100  = 256.00
D1 = 256-100     = 156.00
D2 = 512.25-100  = 412.25
D3 = 2048.88-100 = 1948.88

The actual time taken to reach the endpoint will be:
	distance = sqrt( D0*D0 + D1*D1 + D2*D2 + D3*D3 + D4*D4 )
	Path Time = Requested travel_speed / eclidian distance
*/

void calculate_position_deltas( float mDestination[4], float& delta[4] )
{
	delta[0] = ( mDestination[0] - EncoderCount[0] );
	delta[1] = ( mDestination[1] - EncoderCount[1] );
	delta[2] = ( mDestination[2] - EncoderCount[1] );
	delta[3] = ( mDestination[3] - EncoderCount[1] );
}

void calculate_euclidean_distance( float mDestination[4], float& delta[4] )
{
	float distance = sqrt(
		delta[0]*delta[0] +
		delta[1]*delta[1] +
		delta[2]*delta[2] +
		delta[3]*delta[3]	);
	return distance;
}

void calculate_path_time( float mtravel_speed, float& distance )
{
	float time = travel_speed / distance;
}

/* */
void calculate_velocities( float delta[4], float travel_speed )
{
	delta[0] = ( mDestination[0] - EncoderCount[0] );
	delta[1] = ( mDestination[1] - EncoderCount[1] );
	delta[2] = ( mDestination[2] - EncoderCount[1] );
	delta[3] = ( mDestination[3] - EncoderCount[1] );
}


