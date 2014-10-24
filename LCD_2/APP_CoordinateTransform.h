
/* Use by calling CT_CalculateForwardEquations() each time the ServoN 
   motors have been updated.  When displaying coordinates only, just call before
   drawing the display. 
   
   This utility references the Servo1 variables from the DEV_OCpwm.h device driver.
*/

/********************************************************************
********************	DEFINITIONS   *******************************
*********************************************************************/
#define BASE_SERVO 			Servo1
#define SHOULDER_SERVO		Servo2
#define ELBOW_SERVO			Servo3
#define WRIST_SERVO			Servo4
#define WRIST_ROTATE_SERVO	SERVO_5
#define GRIPPER_SERVO		SERVO_6

#define BASE_SERVO_NUM 	   		1
#define SHOULDER_SERVO_NUM 		2
#define ELBOW_SERVO_NUM    		3
#define WRIST_SERVO_NUM    		4
#define WRIST_ROTATE_SERVO_NUM 	5
#define GRIPPER_SERVO_NUM 		6


/********************************************************************
********************	CALIBRATION DATA   **************************
*********************************************************************/
// The following must be measured (or approximated)
// It shows the number of degrees spanned from the 
//		MIN_SERVO_n_PW to the MIN_SERVO_n_PW.  
// This will be used to convert the pulse width to an actual angle 
// in radians or degrees.  
//
//Robot arm consists only of 6 servos!
#define MAX_DEGREES_SERVO_1 180
#define MAX_DEGREES_SERVO_2 180
#define MAX_DEGREES_SERVO_3 180
#define MAX_DEGREES_SERVO_4 180
#define MAX_DEGREES_SERVO_5 180
#define MAX_DEGREES_SERVO_6 180
#define MAX_DEGREES_SERVO_7 180
#define MAX_DEGREES_SERVO_8 180

// Following are the additonal timer counts required to put the segment  
// in Horizontal position.  For example, Shoulder, Elbow, and Wrist.
// See the OneNote document for diagrams.
#define ZERO_OFFSET_COUNTS_SERVO_1  0x0600
#define ZERO_OFFSET_COUNTS_SERVO_2  0x0600
#define ZERO_OFFSET_COUNTS_SERVO_3  0x0C13   // (3183 - 1536)/180*170 = 1555+1536 = 3091   // make around 170deg
#define ZERO_OFFSET_COUNTS_SERVO_4  0x0600
#define ZERO_OFFSET_COUNTS_SERVO_5  0x0600
#define ZERO_OFFSET_COUNTS_SERVO_6	0x0600
#define ZERO_OFFSET_COUNTS_SERVO_7	0x0600
#define ZERO_OFFSET_COUNTS_SERVO_8	0x0600


// THE FOLLOWING ARE REAL MEASUREMENTS! (expressed in inches)
#define LENGTH_SHOULDER 	4.75
#define LENGTH_ELBOW		4.75
#define LENGTH_WRIST		6.0
#define BASE_HEIGHT			2.5
// THE ABOVE ARE REAL MEASUREMENTS! 	(expressed in inches)


/* Don't use these since they conflict with the Remote 
   Controls L1, L2, R1,R2 buttons.
#define L1	LENGTH_SHOULDER
#define L2	LENGTH_ELBOW
#define L3	LENGTH_WRIST
*/



/********************************************************************
********************  INTERMEDIATE DATA   ***************************
*********************************************************************/
// Following are calculation based on the CAL data above.
#define MAX_RADIANS_SERVO_1 (((float)MAX_DEGREES_SERVO_1*(float)PI)/180.0)
#define MAX_RADIANS_SERVO_2 (((float)MAX_DEGREES_SERVO_2*(float)PI)/180.0)
#define MAX_RADIANS_SERVO_3 (((float)MAX_DEGREES_SERVO_3*(float)PI)/180.0)
#define MAX_RADIANS_SERVO_4 (((float)MAX_DEGREES_SERVO_4*(float)PI)/180.0)
#define MAX_RADIANS_SERVO_5 (((float)MAX_DEGREES_SERVO_5*(float)PI)/180.0)
#define MAX_RADIANS_SERVO_6 (((float)MAX_DEGREES_SERVO_6*(float)PI)/180.0)
#define MAX_RADIANS_SERVO_7 (((float)MAX_DEGREES_SERVO_7*(float)PI)/180.0)
#define MAX_RADIANS_SERVO_8 (((float)MAX_DEGREES_SERVO_8*(float)PI)/180.0)


extern float X,Y,Z;			// THE ARM COORDINATES.

// UTILITY FUNCTIONS:
float 	CT_Convert_Radians_to_Degrees(float mRadians);
float 	CT_Convert_Degrees_to_Radians(float mRadians);
float 	CT_Convert_FlipDirection(float mRadians);
float 	CT_Convert_Servo_to_Radians( U16 mServoPos, U8 mServoNum ); 
float 	CT_Convert_Servo_to_Degrees( U16 mServoPos, U8 mServoNum ); 

void 	CT_CalculateShoulder();		// Stage 1
void 	CT_CalculateElbow();		// Stage 2
void	CT_CalculateWrist();		// Stage 3
void	CT_Calculate_XYZ();			// Stage 4


// Do all functions here:
void CT_CalculateForwardEquations();



