/*********************************************************************
Accelerometer SPI Interface		LIS3D chip
Let XYZ represent translational postion
Let ABC represent rotational angles about XYZ.

6 Degrees of Freedom : XYZ,ABC specifies all 6 degrees of freedom
for each degree, there are Position, Speed, and Acceleration.

Gives a total of 6x3 = 18 unknown quantities.
 
DATE 	:  9/28/2013
AUTHOR	:  Stephen Tenniswood
Product of Beyond Kinetics, Inc
********************************************************************/
#include <avr/sfr_defs.h>
#include <math.h>
#include "bk_system_defs.h"
#include "iom16m1.h"
#include "pin_definitions.h"
#include "spi.h"
#include "can.h"
#include "can_msg.h"
#include "leds.h"
#include "accelerometer.h"
#include "gyro.h"
#include "magnetometer.h"
#include "calc_orientation.h"
#include "calibrations.h"
#include "configuration.h"


/*********** BEST VALUES (Highest Confidence Values) ********************
Incoming data is stored in uXYZ unions:
	gyro
	accel
	magnet
 ************************************************************************/
 
/*********** BEST VALUES (Highest Confidence Values) ********************/
uXYZ BestLinearAccel;
uXYZ BestAngleAccel;

uXYZ BestLinearSpeed;
uXYZ BestAngleSpeed ;

uXYZ BestLinearPosition;
uXYZ BestAnglesPosition;
/************************************************************************/
/******************* INTERMEDIATE CALCULATIONS **************************/
uXYZ accel_angles;				// when all acceleration is due to gravity - converts to angles
uXYZ accel_delta_angles;		// Difference of BestAngle and latest Angle
uXYZ gyro_integrated_angles;	// Integration of gyros (since init)

/********************************************************
 orient_init()
PURPOSE: main init for all code to calculate orientation	
*********************************************************/
void orient_init()
{
	gyro_integrated_angles.coords.x = 0;
	gyro_integrated_angles.coords.y = 0;
	gyro_integrated_angles.coords.z = 0;

	// MORE TO COME...
}

#define near_zero 0x00FF

void orient_timeslice()
{
	// NEW DATA MUST BE AVAILABLE BEFORE CALLING THIS.

	// INTEGRATE GYRO to get ANGLES:
	/*integrate_gyro_angles();
	
	int magn = magnitude(gyro);
	if (compare(magn, 0, near_zero))	// gyro readings magnitude should be near zero here.
	{
		// Can calculate angles w.r.t to gravity from the accelerometer:	
		accel_calc_angles();
		// Average this with the gyro accumulated values (reduce drift)

		// ERR: Removed so it compiles.
		//BestAnglesPosition = average( accel_angles, gyro_integrated_angles );		
	}
	else {
		accumulate( BestAnglesPosition, gyro );		
	}

	//uXYZ tmpXYZ;
	// ANGULAR:
	if (isConfigured(MODE_SEND_COMBINED_ANGLES))  {
		//tmpXYZ.coords = gyro_integrated_angles;
		//can_prep_total_position_msg( &msg1, gyro_integrated_angles );
		can_send_msg_no_wait(0, &msg1);
	}
	if (isConfigured(MODE_SEND_COMBINED_ANGLES_VELOCITY))  {
		//can_prep_total_angular_position_msg( &msg1, gyro.coords);
		can_send_msg_no_wait(2, &msg1);
	}

	// LINEAR:
	if (isConfigured(MODE_SEND_COMBINED_LINEAR))  {
		//can_prep_total_position_msg( &msg1, union uXYZ* mData );	
		can_send_msg_no_wait(0, &msg1);
	}
	if (isConfigured(MODE_SEND_COMBINED_LINEAR_VELOCITY))  {
		//can_prep_total_velocity_msg( &msg1, );
		can_send_msg_no_wait(2, &msg1);		
	} */
	//int magnitude = magnitude(accel);
}

/************************************************************************
 *************************** GYRO CALCS  ********************************
 ************************************************************************/
void integrate_gyro_angles( )
{
	/*gyro_integrated_angles.coords.x += gyro.coords.x;
	gyro_integrated_angles.coords.y += gyro.coords.y;
	gyro_integrated_angles.coords.z += gyro.coords.z;	*/
	// Problem is not initialized upon startup, assumed to be zeros
}

/************************************************************************
 ******************** ACCELEROMETER CALCS  ******************************
 ************************************************************************/
short one_g = 0x4000;  // For 2G sensitivity, 1g is half scale. 

//
// Can only calculate 2 axes based on gravity.  Can't tell Z (or any axis rotated into Z direction)
void accel_calc_angles()
{
//	float xs = (accel.coords.x / one_g);
//	float ys = (accel.coords.y / one_g);
	//float angle_x = asin(xs);
	//float angle_y = asin(ys);

/*	accel_angles.coords.x;
	accel_angles.coords.y;
	accel_angles.coords.y; */
}

void difference_accel_angles()
{
	// delta between BestAngles and latest calculation of Angles.
/*	accel_delta_angles.coords.x = (BestAnglesPosition.coords.x - accel_angles.coords.x);
	accel_delta_angles.coords.y = (BestAnglesPosition.coords.y - accel_angles.coords.y);
	accel_delta_angles.coords.z = (BestAnglesPosition.coords.z - accel_angles.coords.z);
*/}
 
/*void UpdateTranslateVelocities	( word mAccelX, word mAccelX, word mAccelX );
void UpdateAngularVelocities	( word mAccelX, word mAccelX, word mAccelX );
void UpdateTranslatePosition( word mVelocityX, word mVelocityX, word mVelocityX );
void UpdateAngularPosition	( word mAccelX, word mAccelX, word mAccelX );
*/

/************************************************************************
 ******************************* UTILITY   ******************************
 ************************************************************************/
void accumulate( uXYZ &mValue1, uXYZ mDelta )
{
	mValue1.coords.x += mDelta.coords.x;
	mValue1.coords.y += mDelta.coords.y;
	mValue1.coords.z += mDelta.coords.z;
}
 
BOOL compare( int mIndependantValue1, int mMin, int mMax )
{
	if ((mIndependantValue1 > mMin) &&
		(mIndependantValue1 < mMax))
		return TRUE;	
	return FALSE;	
}
BOOL compare_within( int mIndependantValue1, int mDependantValue2, float mPercentError )
{
	short ten_percent = ((float)mIndependantValue1 * mPercentError);  // 10% margin of error.
	short lower_threshold_1g = mIndependantValue1 - ten_percent;  
	short upper_threshold_1g = mIndependantValue1 + ten_percent;  	
	return compare( mIndependantValue1, lower_threshold_1g, upper_threshold_1g );
}

BOOL compare_magnitude_to_gravity( int mMagnitude )
{
	return compare( one_g, mMagnitude, 0.01 );
}

uXYZ average( uXYZ mRead1, uXYZ mRead2 )
{
	static uXYZ avg;
	avg.coords.x = (mRead1.coords.x + mRead2.coords.x) / 2;
	avg.coords.y = (mRead1.coords.y + mRead2.coords.y) / 2;
	avg.coords.z = (mRead1.coords.z + mRead2.coords.z) / 2;
	return avg;
}

int magnitude( uXYZ mReadings )
{
	int magnitude = ((mReadings.coords.x * mReadings.coords.x) +
	 (mReadings.coords.y * mReadings.coords.y) +
	 (mReadings.coords.z * mReadings.coords.z));
	return sqrt(magnitude);
}

