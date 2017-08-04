#ifndef _DIFFERENTIAL_DRIVE_H_
#define _DIFFERENTIAL_DRIVE_H_

#include "global.h"


struct stDiffConfig
{
	float	WheelCircumference;
	float	WheelBaseSeparation;	// a diameter
	float	EncoderCountsPerRev;	
	int		RightWheelIndex;
	int		LeftWheelIndex;
};


void init_differential	();


/* Setup Robot Configuration.  Based on command lines cmds (see protocol.h) */
void set_circumference	( float Circumference );
void set_wheel_base 	( float Circumference );
void set_counts_per_rev	( float Circumference );


// Queues positions for the robot rotation.  Does not block.
void spin_robot_degrees ( float Degrees  );
void move_n_counts		( float Counts   );
void move_inch			( float Distance );


#endif
