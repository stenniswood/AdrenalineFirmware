#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include "configuration.h"


/*********** TYPE DEFINITIONS *******************************************/
// Use this structure for storing the Calibration points
// into EEPROM:
struct sEndPoint
{
	long int angle;		// FPA type;  Degrees*100  
	word 	 value;		// Pot value
};

extern struct sEndPoint 	EndPoint1;
extern struct sEndPoint 	EndPoint2;
extern struct sEndPoint 	Range;			// Computed from EndPoint 1 & 2
//extern struct sStop ExtendedStop1;		// Max Moveable Position
//extern struct sStop ExtendedStop2;		// Max Moveable Position
extern word current_1_threshold;
extern word current_2_threshold;

void app_default_configuration();
void config_change			  (byte mByteChanged);

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
inline byte getBaseFreq() 	{  return ((config_byte_1 & 0xC0)>>6);  }
/**** END OF CONFIG BYTE 1 DEFS   ****/
/*************************************/

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
#define MODE_SEND_POSITION_SPEED_CURRENT	0x01 	// Measured Pot/Encoder, CurrentLeft, CurrentRight, Speed
#define MODE_SEND_ANGLE						0x02 	// Calculated quantities: Angle (deg*100), Current (Amps*100)
#define MODE_SEND_STATUS					0x04 	// 

//#define MODE_SEND_POSITION_RAW	0x01 	// Measured Pot/Encoder, CurrentLeft, CurrentRight, Speed
//#define MODE_SEND_POSITION_CALC 0x02 	// Calculated quantities: Angle (deg*100), Current (Amps*100)
//#define MODE_SEND_STATUS		0x04 	// 
inline void SetReportRate(byte rate)	{  config_byte_2 |= rate; };
byte 		getReportRate();
/**** END OF CONFIG BYTE 2 DEFS   ****/
/*************************************/

/*************************************/
/**** CONFIG BYTE 3 - BIT DEFS    ****/
/**** END OF CONFIG BYTE 3 DEFS   ****/
/*************************************/

byte* save_cal( );		// Saves all of the following EEPROM Data
byte* read_cal( );		// Reads all of the following EEPROM Data

byte* save_stops_eeprom				(byte* addr );
byte* read_stops_eeprom				(byte* addr	);
byte* save_currentThresholds_eeprom	(byte* addr	);
byte* read_currentThresholds_eeprom (byte* addr );
byte* save_latest_position_eeprom	(byte* addr	);
byte* read_latest_position_eeprom	(byte* addr	);

#endif
