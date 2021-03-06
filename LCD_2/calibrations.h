#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

/************* CONFIGURATION *****************/
/********* CONFIG BYTE 1 - BIT DEFS **********/
#define MODE_SEND_ACCEL 					 0x01		// 
#define MODE_SEND_GYRO	 					 0x02		// 
#define MODE_SEND_MAGNET 					 0x04		// 
#define MODE_SEND_COMBINED_ANGLES 			 0x08		// 
#define MODE_SEND_COMBINED_ANGLES_VELOCITY   0x10		// 
#define MODE_SEND_COMBINED_LINEAR 			 0x20		// 
#define MODE_SEND_COMBINED_LINEAR_VELOCITY   0x40		// 
#define MODE_RESERVED_3						 0x80		// 

void app_default_configuration();
void config_change( byte mByteChanged );

byte* save_cal();
byte* read_cal();

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

byte getReportRate();


#endif
