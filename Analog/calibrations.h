#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

/************* CONFIGURATION *****************/
/**** CONFIG BYTE 1 - BIT DEFS ****/
#define MODE_ENABLE_ADC_CHIP1				 0x01		// 
#define MODE_ENABLE_ADC_CHIP2				 0x02		// 
#define MODE_ENABLE_ADC_CHIP3				 0x04		// 
#define MODE_ENABLE_ADC_CHIP4 				 0x08		// 
#define MODE_SEND_DERIVATIVES_1 			 0x10		// 
#define MODE_SEND_DERIVATIVES_2 			 0x20		// 
#define MODE_SEND_DERIVATIVES_3				 0x40		// 
#define MODE_SEND_DERIVATIVES_4				 0x80		// 

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

void app_default_configuration();
void config_change( byte mByteChanged );

byte* save_cal();
byte* read_cal();

byte getReportRate();
BOOL isReportingEnabled();


#endif
