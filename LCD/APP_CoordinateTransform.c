/*********************************************************************
 *											   						 *
 *    DESCRIPTION:  Coordinate Transforms 			 	 			 *
 *											  						 *
 *	  CATEGORY	:  Low leve APP										 *
 *											  						 *
 *    AUTHOR	:  Stephen Tenniswood, Embedded Engineer 	 		 *
 *											   						 *
 *    PURPOSE	:  Routines to convert Servo positions into Angles.	 *
 *			   	   and to convert Angles into an <X,Y,Z> coordinate. *
 *																	 *
 *	  DATE		:  Octt 27,2008										 *
 *********************************************************************/
#include <math.h>
#include "skp_bsp.h"
#include "DEV_OCpwm.h"
#include "APP_ArmPositionStorage.h"
#include "APP_CoordinateTransform.h"


/********************************************************************
**********************  VARIABLE DATA   *****************************
*********************************************************************/
// Temporary Calculations arriving at the <x,y,z> vector
float R1,H1;
float R2,H2;
float R3,H3;
float Real_Shoulder;
float Real_Elbow;
float Real_Wrist;

float FinalHeight ;		// the Height above the base.
float FinalRadius ;		// the Radius away from base.
float X,Y,Z;			// THE ARM COORDINATES.


/**********************************************************************************
Name		:   CT_ConvertServoPos_to_Radians()
Parameters	:	Servo counter value
			:	Servo number
Returns 	:   TRUE when finished
Description	:   Take the pulse width value and convert it to radians.
************************************************************************************/
float CT_Convert_Servo_to_Radians( U16 mServoPos, U8 mServoNum )
{
	float RadiansPerCount;
	float Angle;
	// Linearly interpolate between Min and Max.
	switch (mServoNum)
	{
		case 1 :
				// Note SERVO_x_RANGE is the number of timer counts which is in the working range of the servo.
				// In otherwords beyond the MIN and MAX counts no movement of the servo results.
				// Angle = (MAX_RADIANS / MAX_COUNT) * (ServoPos - ZeroOffsetCount)
				Angle = ( (float)MAX_RADIANS_SERVO_1 / (float)SERVO_1_RANGE ) * (((float)mServoPos-ZERO_OFFSET_COUNTS_SERVO_1));
				// Note2:  We subtract the ZERO_OFFSET_COUNTS_SERVO_1 calibration position to horizontal 
				//			rather than the MIN_SERVO_1_PW.  This way the Angle is according to the calibrated
				//			horizontal positions.  See Get_Raw_servo_angle.
				break;
		case 2 :
				Angle = ((float)MAX_RADIANS_SERVO_2 / (float)SERVO_2_RANGE ) * (((float)mServoPos-ZERO_OFFSET_COUNTS_SERVO_2));
				Angle = CT_Convert_FlipDirection( Angle );
				break;
		case 3 :
				Angle = ((float)MAX_RADIANS_SERVO_3 / (float)SERVO_3_RANGE ) * (((float)mServoPos-ZERO_OFFSET_COUNTS_SERVO_3));
				break;
		case 4 :
				Angle = ((float)MAX_RADIANS_SERVO_4 / (float)SERVO_4_RANGE ) * (((float)mServoPos-ZERO_OFFSET_COUNTS_SERVO_4));
				Angle = CT_Convert_FlipDirection( Angle );
				break;
		case 5 :
				Angle = ((float)MAX_RADIANS_SERVO_5 / (float)SERVO_5_RANGE ) * (((float)mServoPos-ZERO_OFFSET_COUNTS_SERVO_5));
				break;
		case 6 :
				Angle = ((float)MAX_RADIANS_SERVO_6 / (float)SERVO_6_RANGE ) * (((float)mServoPos-ZERO_OFFSET_COUNTS_SERVO_6));
				break;
		case 7 :
				Angle = ((float)MAX_RADIANS_SERVO_7 / (float)SERVO_7_RANGE ) * (((float)mServoPos-ZERO_OFFSET_COUNTS_SERVO_7));
				break;
		case 8 :
				RadiansPerCount = ((float)MAX_RADIANS_SERVO_8 / (float)SERVO_8_RANGE );
				Angle = RadiansPerCount * ( (float)(mServoPos-ZERO_OFFSET_COUNTS_SERVO_8)  );
				// Note:  This is not subtracting MIN_SERVO_8_PW which would take us to the 
				// 		  min range of travel.
				// 		  Instead it takes us to the properly CALIBRATED 0 offset.
				break;
		default: 
				break;
	}
	return Angle;
}

/**********************************************************************************
Name		:   CT_Convert_FlipDirection()
Parameters	:	mRadians
Returns 	:   Degrees
Description	:   Switch between ClockWise and CounterClockWise
************************************************************************************/
float CT_Convert_FlipDirection(float mRadians)
{
	float Angle;
	Angle = PI - mRadians;
	return Angle;
}

/**********************************************************************************
Name		:   CT_Convert_Radians_to_Degrees()
Parameters	:	mRadians
Returns 	:   Degrees
Description	:   Take the pulse width value and convert it to radians.
************************************************************************************/
float CT_Convert_Radians_to_Degrees(float mRadians)
{
	float Angle;					// Linearly interpolate between Min and Max.
	Angle = (mRadians * 180.0)/PI;	
	return Angle;
}

/**********************************************************************************
Name		:   CT_Convert_Degrees_to_Radians()
Parameters	:	mRadians
Returns 	:   Degrees
Description	:   Take the pulse width value and convert it to radians.
************************************************************************************/
float CT_Convert_Degrees_to_Radians(float mDegrees)
{
	float Angle;					// Linearly interpolate between Min and Max.
	Angle = (mDegrees * PI)/180.0;
	return Angle;
}

/**********************************************************************************
Name		:   CT_ConvertServoPos_to_Radians()
Parameters	:	mRadians
Returns 	:   Degrees
Description	:   Take the pulse width value and convert it to radians.
************************************************************************************/
float CT_Convert_Servo_to_Degrees( U16 mServoPos, U8 mServoNum )
{
	float tmp = CT_Convert_Servo_to_Radians( mServoPos, mServoNum );
	
	return CT_Convert_Radians_to_Degrees( tmp );
}

/**********************************************************************************
Name		:   CT_CalculateShoulder()
Parameters	:	mRadians
Returns 	:   Degrees
Description	:   See OneNote document.
************************************************************************************/
void CT_CalculateShoulder()
{	
	Real_Shoulder = CT_Convert_Servo_to_Radians( SHOULDER_SERVO, 1 );
	R1 = cos (Real_Shoulder)* LENGTH_SHOULDER;
	H1 = sin (Real_Shoulder)* LENGTH_SHOULDER;
}

/**********************************************************************************
Name		:   CT_CalculateElbow()
Parameters	:	mRadians
Returns 	:   Degrees
Description	:   See OneNote document.
************************************************************************************/
void CT_CalculateElbow()
{	
	Real_Elbow = Real_Shoulder + CT_Convert_Servo_to_Radians( ELBOW_SERVO, 2 );
	
	R2 = cos (Real_Elbow)* LENGTH_ELBOW;
	H2 = sin (Real_Elbow)* LENGTH_ELBOW;
}

/**********************************************************************************
Name		:   CT_CalculateWrist()
Parameters	:	mRadians
Returns 	:   Degrees
Description	:   See OneNote document.
************************************************************************************/
void CT_CalculateWrist()
{	
	Real_Wrist = Real_Elbow + CT_Convert_Servo_to_Radians( WRIST_SERVO, 1 );	
	R3 = cos (Real_Wrist)* LENGTH_WRIST;
	H3 = sin (Real_Wrist)* LENGTH_WRIST;
}

/**********************************************************************************
Name		:   CT_Calculate_XYZ()
Parameters	:	mRadians
Returns 	:   Degrees
Description	:   See OneNote document.
************************************************************************************/
void CT_Calculate_XYZ()
{
	float Real_Base;
	FinalRadius = R1 + R2 + R3;
	FinalHeight = H1 + H2 + H3;

	// BASE_SERVO
	Real_Base = CT_Convert_Servo_to_Radians( BASE_SERVO, 1 );
	X = cos(Real_Base)*FinalRadius;
	Y = sin(Real_Base)*FinalRadius;
	Z = FinalHeight + BASE_HEIGHT;
}

/**********************************************************************************
Name		:   CT_CalculateForwardEquations()
Parameters	:	none
Returns 	:   
Description	:   See OneNote document.
************************************************************************************/
void CT_CalculateForwardEquations()
{
	// Intermediate values are stored in file scope variables.
	CT_CalculateShoulder();
	CT_CalculateElbow();
	CT_CalculateWrist();
	CT_Calculate_XYZ();
}


