#ifndef _MOTOR_H_
#define _MOTOR_H_


/**************** MOTOR STATUS ***********************/
#define MOTOR_OFF 	   		0
#define MOTOR_FORWARD  		1
#define MOTOR_BACKWARD 		2
#define MOTOR_LIMIT_1  		3
#define MOTOR_LIMIT_2  		4
#define MOTOR_OVER_TEMP  	5
#define MOTOR_OVER_VOLTAGE  6
#define MOTOR_DISABLED 		7
#define MOTOR_ENABLED 		8
#define MOTOR_BRAKING 		9
#define MOTOR_REQUEST_PENDING 	10
#define MOTOR_COASTING 		11
extern  byte MotorState;
/*****************************************************/
extern float MotorDutyRequest;	// set in can_msg.c;  [-1.0..+1.0]

void motor_init 		 ( 				 );
void motor_timeslice_10ms(				 );
void report_timeslice_10ms(				 );

void motor_set_duty_signed( float mSignedFraction );
void motor_set_duty 	( float mFraction, BOOL mDirection );

void set_dutyA			( float mDuty 	 );
void set_dutyB			( float mDuty 	 );
void OCR0A_Write		( unsigned int i );
void OCR0B_Write		( unsigned int i );

void enable_left_side 	( BOOL mEnable 	 );
void enable_right_side	( BOOL mEnable 	 );

void  SetBaseFrequency	(	  			 );
void  set_motor_direction( BOOL mForward );
void  motor_enable		( BOOL mEnable 	 );
void  motor_stop		(				 );
void  motor_brake		( 				 );
void  motor_coast		( float mFraction);

float get_motor_angle	(				 );
void  stop_action		( byte mStop	 );


#endif
