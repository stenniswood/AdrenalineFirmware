#ifndef _DIFFERENTIAL_DRIVE_H_
#define _DIFFERENTIAL_DRIVE_H_

#include "global.h"
#include "configuration.h"

void init_differential	();


/* Setup Robot Configuration.  Based on command lines cmds (see protocol.h) */

void set_circumference		( float Circumference );
void set_wheel_separation	( float Circumference );
void set_counts_per_rev		( float Circumference );

float  convert_count_per_unit(enum eMeasuringUnit mFrom, enum eMeasuringUnit mTo);	// called in protocol "set unit" command
float convert_units( enum eMeasuringUnit mFrom, enum eMeasuringUnit mTo, float mValue, char mMotorLetter );

// Queues positions for the robot rotation.  Does not block.
void spin_robot_degrees ( float mDegrees    );
void move_forward		( float  mDistance  );

void curve_left			( float mDegrees, float mRadius, float mSpeed	);
void curve_right		( float mDegrees, float mRadius, float mSpeed	);

void turn_right			( float mDegrees, float mSpeed	);
void turn_left			( float mDegrees, float mSpeed	);


#endif
