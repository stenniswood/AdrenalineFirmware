#ifndef _MOTOR_H_
#define _MOTOR_H_

extern byte config_byte_1 ;
extern byte config_byte_2 ;
extern byte config_byte_3 ;
extern byte config_byte_4 ;
extern byte sys_config_byte;
//byte ShutDownState 		= NO_SHUTDOWN_PENDING;

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

/************* CONFIGURATION *****************/
/**** CONFIG BYTE 1 - BIT DEFS ****/
#define MODE_USE_ENCODER 	0x01		// else use potentiometer.  default is pot (0x00)
#define MODE_2_MOTORS	 	0x02		// 0=> 1 bidirectional;  1=> 2 unidirectional motors
#define MODE_RESERVED_1 	0x04		// else defaults (80%)
#define MODE_FREE_SPIN	 	0x08		// ignore stops.  default (0) is to stop
#define MODE_TILT_RESPONDER 0x10		// 1=> respond to tilt Y axis readings
#define MODE_PID_CONTROL	0x20		// PID or constant speed.
#define MODE_BASE_FREQ_1	0x40		// 00->100Hz;  01--> 300Hz
#define MODE_BASE_FREQ_2	0x80		// 10->1000hz; 11--> 5000Hz
//inline byte getBaseFreq() 	{  return ((config_byte_1 & 0xC0)>>6);  }
/**** END OF CONFIG BYTE 1 DEFS   ****/
/*************************************/
/**** CONFIG BYTE 2 - BIT DEFS    ****/
/* Upper nibble of config_byte_2 defines the update rate.  
   BigMotor will send motor angle and current readings
every:
	0	- No report
	1	- Report every 10ms
	2	- Report every 20ms
	3	- Report every 50ms
	4	- Report every 100ms	*/
#define MODE_SEND_UPDATES_NONE	0x00	// 
#define MODE_SEND_UPDATES_10ms	0x10	// 
#define MODE_SEND_UPDATES_20ms	0x20	// 
#define MODE_SEND_UPDATES_50ms	0x40	// 
#define MODE_SEND_UPDATES_100ms	0x80	// 
// Lower nibble indicates which messages will be reported:
#define MODE_SEND_POSITION_RAW	0x01 	// Measured Pot/Encoder, CurrentLeft, CurrentRight, Speed
#define MODE_SEND_POSITION_CALC 0x02 	// Calculated quantities: Angle (deg*100), Current (Amps*100)
#define MODE_SEND_STATUS		0x04 	// 
inline void SetReportRate(byte rate)	{  config_byte_2 |= rate; };
byte 		getReportRate();
/**** END OF CONFIG BYTE 2 DEFS   ****/
/*************************************/

/************* CONFIGURATION *****************/

/*inline byte isConfigured (byte Test)	{  return ((config_byte_1 & Test)>0); };
inline byte isConfigured2(byte Test)	{  return ((config_byte_2 & Test)>0); };
inline byte isConfigured3(byte Test)	{  return ((config_byte_3 & Test)>0); };
inline byte isConfigured4(byte Test)	{  return ((config_byte_4 & Test)>0); };
inline byte isSysConfigured(byte Test)	{  return ((sys_config_byte & Test)>0); };*/



void motor_init 	( 				 );
void motor_timeslice_10ms();

void motor_set_duty_signed( float mSignedFraction );
void motor_set_duty ( float mFraction);
void set_dutyA		( float mDuty 	 );
void set_dutyB		( float mDuty 	 );
void OCR0A_Write	( unsigned int i );
void OCR0B_Write	( unsigned int i );

void enable_left_side ( BOOL mEnable );
void enable_right_side( BOOL mEnable );

void  SetBaseFrequency	 (  			 );
void  set_motor_direction( BOOL mForward );
void  motor_enable		 ( BOOL mEnable  );
void  motor_stop		 (				 );
void  motor_brake		 ( BOOL mEnable  );
float get_motor_angle	 (				 );
void  stop_action		 ( word mPosition );


#endif
