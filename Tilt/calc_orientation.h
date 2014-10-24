#ifndef _CALC_ORIENTATION_H_
#define _CALC_ORIENTATION_H_

#include "can_msg.h"

/*********** BEST VALUES (Highest Confidence Values) ********************/
extern uXYZ BestLinearAccel;
extern uXYZ BestAngleAccel ;

extern uXYZ BestLinearSpeed;
extern uXYZ BestAngleSpeed ;

extern uXYZ BestLinearPosition;
extern uXYZ BestAnglesPosition;
/************************************************************************/

void orient_init				( );
void orient_timeslice			( );
void integrate_gyro_angles		( );
void accel_calc_angles			( );
void difference_accel_angles	( );

void UpdateTranslateVelocities	( word mAccelX, 	word mAccelY, word mAccelZ );
void UpdateAngularVelocities	( word mAccelX, 	word mAccelY, word mAccelZ );
void UpdateTranslatePosition	( word mVelocityX, 	word mVelocityY, word mVelocityZ );
void UpdateAngularPosition		( word mAccelX, 	word mAccelY, word mAccelZ );

uXYZ average						( uXYZ mRead1, uXYZ mRead2 );
void accumulate					( uXYZ &mValue1, uXYZ mDelta );
BOOL compare					( int mIndependantValue1, int mMin, int mMax );
BOOL compare_within				( int mIndependantValue1, int mDependantValue2, float mPercentError );
BOOL compare_magnitude_to_gravity( int mMagnitude );
int magnitude					( uXYZ mReadings );


#endif
