#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include "configuration.h"

/*********** TYPE DEFINITIONS *******************************************/
// Use this structure for storing the Calibration points
// into EEPROM:
struct sEndPoint
{
	long int angle;		// FPA type;  Degrees * 100
	word 	 value;		// Pot value
};

#define CONFIGURATION_EEPROM_ADDRESS 			INSTANCE_EEPROM_ADDRESS+1
#define CALIBRATION_STOPS_EEPROM_START_ADDRESS  CONFIGURATION_EEPROM_ADDRESS+2
#define CALIBRATION_EEPROM_CURRENT_ADDRESS 		CALIBRATION_STOPS_EEPROM_START_ADDRESS+(2*sizeof(sEndPoint))

extern struct sEndPoint 	EndPoint1;
extern struct sEndPoint 	EndPoint2;
extern struct sEndPoint 	Range;			// Computed from EndPoint1 & 2
//extern struct sStop ExtendedStop1;		// Max Moveable Position
//extern struct sStop ExtendedStop2;		// Max Moveable Position
extern word current_1_threshold;
extern word current_2_threshold;


/************* CONFIGURATION *****************/
/**** CONFIG BYTE 1 - BIT DEFS ****/
#define MODE_USE_ENCODER 	0x01		// else use potentiometer.  default is pot (0x00)
#define MODE_STOP_1_STORED 	0x02		// else defaults (20%)
#define MODE_STOP_2_STORED 	0x04		// else defaults (80%)
#define MODE_FREE_SPIN	 	0x08		// ignore stops.  default (0) is to stop
#define MODE_TILT_RESPONDER 0x10		// 1=> respond to tilt Y axis readings
#define MODE_PID_CONTROL	0x20		// PID or constant speed.
#define MODE_RESERVED_2		0x40		// 1=> respond to tilt Y axis readings
#define MODE_RESERVED_3		0x80		// 1=> respond to tilt Y axis readings

/**** CONFIG BYTE 2 - BIT DEFS ****/
/* Upper nibble of config_byte_2 defines the update rate.  
   BigMotor will send motor angle and current readings
every:
	0	- No report
	1	- Report every 10ms
	2	- Report every 20ms
	3	- Report every 50ms
	4	- Report every 100ms	*/
#define MODE_SEND_UPDATES_NONE	0x00		// 
#define MODE_SEND_UPDATES_10ms	0x10		// 
#define MODE_SEND_UPDATES_20ms	0x20		// 
#define MODE_SEND_UPDATES_50ms	0x40		// 
#define MODE_SEND_UPDATES_100ms	0x80		// 
// Lower nibble indicates which messages will be reported:
#define MODE_SEND_POSITION_RAW	0x01 // Measured Pot/Encoder, CurrentLeft, CurrentRight, Speed
#define MODE_SEND_POSITION_CALC 0x02 // Calculated quantities: Angle (deg*100), Current (Amps*100)
#define MODE_SEND_STATUS		0x04 // 
/*************************************/

inline void SetReportRate(byte rate)	{  config_byte_2 |= rate; };
byte 		getReportRate();


byte* save_stops_eeprom				(byte* addr );
byte* read_stops_eeprom				(byte* addr	);
byte* save_currentThresholds_eeprom	(byte* addr	);
byte* read_currentThresholds_eeprom (byte* addr );

byte* save_latest_position_eeprom	(byte* addr	);
byte* read_latest_position_eeprom	(byte* addr	);
// these prototypes are in configuration.h
//byte* save_cal						(byte* addr = (byte*)(INSTANCE_EEPROM_ADDRESS+1));		// Save to EEPROM
//byte* read_cal						(byte* addr	= (byte*)(INSTANCE_EEPROM_ADDRESS+1));		// Read from EEPROM	


void mark_stop_1( float mAngle );	// Save to EEPROM
void mark_stop_2( float mAngle );	// Save to EEPROM



#endif
